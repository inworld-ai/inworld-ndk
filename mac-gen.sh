mkdir build
cd build
cmake -G Xcode .. -DAEC=True -DMAC=True -DINWORLD_LOG_SPD=True -DSTT_LOCAL=True #-DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" 
