mkdir build
cd build
cmake -G Xcode .. -DAEC=True -DMAC=True -DINWORLD_LOG_SPD=True -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -DINWORLD_SHARED=True
