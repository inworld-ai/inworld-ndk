mkdir build
cd build
cmake --build . --target inworld-ndk-vad --config Release
mv ./../prebuilt/Mac/libinworld-ndk-vad.dylib ./../prebuilt/Mac/libinworld-ndk-vad-arm64.dylib