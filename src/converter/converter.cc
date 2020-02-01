#include "converter.hh"

Converter::Converter(string path_in, string path_out)
: out(path_out)
{
    unsigned colors = 8;
    srand(128);
    this->image = fipImage();
    this->image.load(path_in.c_str());
    if (!this->image.isValid()) {
        cerr << path_in << ": File is invalid" << endl;
        exit(1);
    }
    unsigned width = this->image.getWidth();
    unsigned height = this->image.getHeight();

    this->clusters = vector<RGBQUAD>(colors);
    
    for (unsigned i = 0; i < colors; ++i) {
        this->clusters[i].rgbRed = i < 4 ? 0 : 255;
        this->clusters[i].rgbGreen = (i % 4) < 2 ? 0 : 255;
        this->clusters[i].rgbBlue = i % 2 == 0 ? 0 : 255;
    }

    vector<unsigned> counts = vector<unsigned>(colors, 0);
    vector<vector<double>> avgs = vector<vector<double>>(colors, vector<double>(3, 0));

    for (unsigned x = 0; x < 10; ++x) {
        for (unsigned i = 0; i < width; ++i) {
            for (unsigned j = 0; j < height; ++j) {
                RGBQUAD color;
                this->image.getPixelColor(i, j, &color);
                int cluster = this->get_cluster(color);
                counts[cluster]++;
                avgs[cluster][0] += color.rgbRed;
                avgs[cluster][1] += color.rgbGreen;
                avgs[cluster][2] += color.rgbBlue;
            }
        }
        for (unsigned c = 0; c < colors; ++c) {
            if (counts[c] > 0) {
                this->clusters[c].rgbRed = (int)(avgs[c][0] / counts[c]);
                this->clusters[c].rgbGreen = (int)(avgs[c][1] / counts[c]);
                this->clusters[c].rgbBlue = (int)(avgs[c][2] / counts[c]);
            }
        }
        counts = vector<unsigned>(colors, 0);
        avgs = vector<vector<double>>(colors, vector<double>(3, 0));
    }
}

bool Converter::convert() {

    unsigned kernel_size = 5;

    // Average pooling (applies blur)
    if (!this->average_pooling(this->image, kernel_size)) {
        cerr << "Invalid pooling k" << endl;
        return false;
    }
    
    // Pooling
    vector<vector<int>> m = this->map_image(this->image);
    if (!this->pooling(m, kernel_size)) {
        cerr << "Invalid pooling k" << endl;
        return false;
    }
    
    // Generation and writing of the image
    unsigned bpp = this->image.getBitsPerPixel();
    fipImage image_out = this->generate_image(m, bpp);
    image_out.save(FIF_PNG, this->out.c_str());
    return true;
}

double Converter::distance(RGBQUAD& color, int cluster) {
    RGBQUAD cluster_color = this->clusters[cluster];
    double red = (cluster_color.rgbRed - color.rgbRed) * (cluster_color.rgbRed - color.rgbRed);
    double green = (cluster_color.rgbGreen - color.rgbGreen) * (cluster_color.rgbGreen - color.rgbGreen);
    double blue = (cluster_color.rgbBlue - color.rgbBlue) * (cluster_color.rgbBlue - color.rgbBlue);
    return sqrt(red + green + blue);
}

int Converter::get_cluster(RGBQUAD& color) {
    int cluster = 0;
    int distance = this->distance(color, 0);
    for (unsigned i = 1; i < this->clusters.size(); ++i) {
        double dd = this->distance(color, i);
        if (dd < distance) {
            distance = dd;
            cluster = i;
        }
    }
    return cluster;
}

vector<vector<int>> Converter::map_image(const fipImage& image) {
    unsigned width = image.getWidth();
    unsigned height = image.getHeight();
    vector<vector<int>> m = vector<vector<int>>(width, vector<int>(height));
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            image.getPixelColor(i, j, &color);
            m[i][j] = this->get_cluster(color);
        }
    }
    return m;
}

fipImage Converter::generate_image(const vector<vector<int>>& m, unsigned bpp) {
    unsigned width = m.size();
    unsigned height = m[0].size();
    fipImage ret = fipImage(FIT_BITMAP, width, height, bpp);
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color = this->clusters[m[i][j]];
            ret.setPixelColor(i, j, &color);
        }
    }
    ret.convertTo24Bits();
    return ret;
}

bool Converter::pooling(vector<vector<int>>& m, int k) {
    int width = m.size();
    int height = m[0].size();
    vector<vector<int>> ret = vector<vector<int>>(width, vector<int>(height));
    if (k % 2 == 0 || k < 0)
        return false;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            vector<int> keys = vector<int>(this->clusters.size(), 0);
            for (int x = -k/2; x <= k/2; ++x) {
                for (int y = -k/2; y <= k/2; ++y){
                    if (i + x >= 0 && i + x < width && j + y >= 0 && j + y < height){
                        keys[m[i+x][j+y]] += 1;
                    }
                }
            }
            int cluster = 0;
            int bcluster  = 0;
            for (unsigned i = 0; i < keys.size(); ++i) {
                if (keys[i] > bcluster) {
                    cluster = i;
                    bcluster = keys[i];
                }
            }
            ret[i][j] = cluster;
        }
    }
    m = ret;
    return true;
}

bool Converter::average_pooling(fipImage& image, int k) {
    fipImage ret = fipImage(image);
    int width = ret.getWidth();
    int height = ret.getHeight();
    double size = k * k;
    if (k % 2 == 0 || k < 0)
        return false;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            double rmean = 0;
            double gmean = 0;
            double bmean = 0;
            for (int x = -k/2; x <= k/2; ++x) {
                for (int y = -k/2; y <= k/2; ++y){
                    if (i + x >= 0 && i + x < width && j + y >= 0 && j + y < height){
                        RGBQUAD color;
                        image.getPixelColor(i + x, j + y, &color);
                        rmean += color.rgbRed;
                        gmean += color.rgbGreen;
                        bmean += color.rgbBlue;
                    }
                }
            }
            RGBQUAD color_set;
            color_set.rgbRed = rmean / size;
            color_set.rgbGreen = gmean / size;
            color_set.rgbBlue = bmean / size;
            ret.setPixelColor(i, j, &color_set);
        }
    }
    image = ret;
    return true;
}

void Converter::print_clusters() {
    for (unsigned i = 0; i < this->clusters.size(); ++i) {
        cout << "Cluster " << i << " R = " << (int)this->clusters[i].rgbRed << " G = ";
        cout << (int)this->clusters[i].rgbGreen << " B = " << (int)this->clusters[i].rgbGreen << endl;
    }
}