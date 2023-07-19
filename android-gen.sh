mkdir build
cd build
cmake .. -DAEC=False\
 -DANDROID=True\
 -DCMAKE_SYSTEM_NAME=Android\
 -DCMAKE_SYSTEM_VERSION=31\
 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a\
 -DCMAKE_ANDROID_NDK=/Users/artem/Library/Android/sdk/ndk/25.1.8937393\
 -DCMAKE_BUILD_TYPE=Debug