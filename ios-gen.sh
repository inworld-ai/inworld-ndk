mkdir build
cd build
cmake -G Xcode .. -DAEC=False -DIOS=True -DCMAKE_TOOLCHAIN_FILE=./ios.toolchain.cmake -DPLATFORM=OS64 -DINWORLD_SHARED=True
