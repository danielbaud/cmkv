#!/bin/bash

make -s
rm out.png &> /dev/null
pngdir="png"

if [ $# -ne 1 ]; then    
    for image in $(ls $pngdir); do
        ./cmkv $pngdir/$image out.png
        feh $pngdir/$image out.png
        rm out.png
    done
    exit 0
fi

./cmkv $1 out.png
feh $1 out.png
rm out.png
