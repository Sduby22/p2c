#!/bin/sh

set -e 

mkdir -p bin
rm -rf bin/*

# build wasm
mkdir -p build && cd build
rm -rf *
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8
mv libparser* ../bin
cd ..

# build cli
mkdir -p build && cd build
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
make -j8
mv *main ../bin