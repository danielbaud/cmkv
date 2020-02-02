#include "converter.hh"


// Constructor
Converter::Converter(string path_in, string path_out)
: out(path_out)
{
    // Number of colors to reduce to
    unsigned colors = 8;
    
    // Size of Sphere to stop the convergence of kmeans
    double diff_threshold = 5;

    // Image loading
    this->image = fipImage();
    this->image.load(path_in.c_str());
    if (!this->image.isValid()) {
        cerr << path_in << ": File is invalid" << endl;
        exit(1);
    }
    unsigned width = this->image.getWidth();
    unsigned height = this->image.getHeight();
    this->image.convertTo24Bits();

    // Clusters initialization
    this->clusters = vector<RGBQUAD>(colors);
    
    for (unsigned i = 0; i < colors; ++i) {
        this->clusters[i].rgbRed = i < colors / 2 ? 0 : 255;
        this->clusters[i].rgbGreen = (i % colors / 2) < colors / 4 ? 0 : 255;
        this->clusters[i].rgbBlue = i % 2 == 0 ? 0 : 255;
    }

    // Kmeans Algorithm
    double update_diff = 100;

    while (update_diff > diff_threshold) {
        auto counts = vector<unsigned>(colors, 0);
        auto avgs = vector<vector<double>>(colors, vector<double>(3, 0));
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
        update_diff = 0;
        for (unsigned c = 0; c < colors; ++c) {
            RGBQUAD cluster = this->clusters[c];
            if (counts[c] > 0) {
                this->clusters[c].rgbRed = (int)(avgs[c][0] / counts[c]);
                this->clusters[c].rgbGreen = (int)(avgs[c][1] / counts[c]);
                this->clusters[c].rgbBlue = (int)(avgs[c][2] / counts[c]);
            }
            update_diff += this->distance(cluster, c);
        }
        update_diff /= 8;
    }
}

// Function that writes the converted image
bool Converter::convert() {

    // Kernel size, 7 is great
    unsigned kernel_size = 7;

    // Average pooling (applies blurr)
    if (!this->average_pooling(kernel_size)) {
        cerr << "Invalid pooling k" << endl;
        return false;
    }
    
    // Pooling
    vector<vector<int>> m = this->map_image();
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

// Computes the distance between a color and the cluster color
double Converter::distance(RGBQUAD& color, int cluster) {
    RGBQUAD cluster_color = this->clusters[cluster];
    double red = cluster_color.rgbRed - color.rgbRed;
    red *= red;
    double green = cluster_color.rgbGreen - color.rgbGreen;
    green *= green;
    double blue = cluster_color.rgbBlue - color.rgbBlue;
    blue *= blue;
    return sqrt(red + green + blue);
}

// Returns the closest cluster of the color
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

// Maps a image with clusters instead
vector<vector<int>> Converter::map_image() {
    unsigned width = this->image.getWidth();
    unsigned height = this->image.getHeight();
    vector<vector<int>> m = vector<vector<int>>(width, vector<int>(height));
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            this->image.getPixelColor(i, j, &color);
            m[i][j] = this->get_cluster(color);
        }
    }
    return m;
}

// Generates a image according to the clusters mapped
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

// Changes a pixel cluster according to its surroundings
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

// Blurring applied to a image
bool Converter::average_pooling(int k) {
    fipImage ret = fipImage(this->image);
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