mkdir build
cd build
cmake -G Xcode .. -DAEC=False -DUNITY=True -DIOS=True -DINWORLD_LOG_CALLBACK=True -DCMAKE_TOOLCHAIN_FILE=./ios.toolchain.cmake -DPLATFORM=OS64
