mkdir build
cd build
cmake .. -DAEC=False\
 -DANDROID=True\
 -DINWORLD_LOG=True\
 -DINWORLD_SHARED=True\
 -DCMAKE_SYSTEM_NAME=Android\
 -DCMAKE_SYSTEM_VERSION=33\
 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a\
 -DCMAKE_ANDROID_NDK=/Users/runner/Library/Android/sdk/ndk/26.3.11579264