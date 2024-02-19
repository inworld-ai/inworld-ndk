mkdir build
cd build
cmake --build . --target inworld-ndk-app --config Release
cmake --build . --target inworld-ndk-unit --config Release

cd Release
./inworld-ndk-unit