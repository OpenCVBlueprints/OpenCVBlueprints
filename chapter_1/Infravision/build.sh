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
cp -f $lib/*.so* build

g++ Infravision.cpp -o build/Infravision -std=c++11 \
  -I include -I $OPENNI2_INCLUDE -L $lib -L $OPENNI2_REDIST \
  -Wl,-R\$ORIGIN -Wl,-R$OPENNI2_REDIST -Wl,-R$OPENNI2_REDIST/OpenNI2 \
  -lopencv_core -lopencv_highgui -lopencv_imgproc -lOpenNI2
