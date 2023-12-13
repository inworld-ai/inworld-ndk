mkdir build
cd build
cmake -G Xcode .. -DMAC=True #-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
