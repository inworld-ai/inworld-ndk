mkdir build
cd build
cmake --build . --target inworld-ndk-vad --config Release
mv ./../prebuilt/Mac/libinworld-ndk-vad.dylib ./../prebuilt/Mac/libinworld-ndk-vad-x86_64.dylib