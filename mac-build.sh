mkdir build
cd build
cmake --build . --target InworldNDKApp --config Release
cmake --build . --target InworldNDKUnit --config Release

cd Release
./InworldNDKUnit