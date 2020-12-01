#!/bin/bash

mkdir -p release

echo "Configuring and building lirs-wct-console ..."
cd lirs_wct_console
mkdir -p build
cd build
cmake ..
make -j
cp lirs_wct_console ../../release

cd ../..

echo "Configuring and building lirs-wct-gui ..."
cd lirs_wct_gui
mkdir -p build
cd build
cmake ..
make -j
cp lirs_wct_gui ../../release


