#!/bin/bash

if [ $# -ne 1 ]; then
    echo "image.png ??"
    exit 1
fi

make -s

rm out.png &> /dev/null
./cmkv $1 out.png
feh $1 out.png
rm out.png
