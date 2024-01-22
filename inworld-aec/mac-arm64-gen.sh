mkdir build
cd build
cmake -G Xcode .. -DMAC=True -DCMAKE_OSX_ARCHITECTURES="arm64"
