#!/bin/bash

arch=$(uname -m)
if [ "$arch" == "i686" -o "$arch" == "i386" -o "$arch" == "i486" -o "$arch" == "i586" ]; then
  lib=lib/x86
elif [ "$arch" == "x86_64" -o "$arch" == "amd64" ]; then
  lib=lib/x64
else
  lib="lib/$arch"
fi

mkdir -p build
cp -f data/* build
cp -f $lib/*.so* build

g++ LookSpry.cpp -o build/LookSpry -std=c++11 -I include -L $lib \
  -Wl,-R\$ORIGIN \
  `sdl2-config --cflags --libs` -lflycapture-c \
  -lopencv_core -lopencv_imgproc -lopencv_objdetect
