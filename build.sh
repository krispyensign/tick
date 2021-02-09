#!/bin/bash -ex
rm -fr build/
mkdir -p build/
cd build/
conan install ..
cmake ..
cp compile_commands.json ..
make all
