mkdir build
cd build
cmake -G Xcode .. -DAEC=True -DMAC=True -DUNITY=True -DINWORLD_LOG_CALLBACK=True -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
