mkdir build
cd build
cmake .. -DAEC=False\
 -DANDROID=True\
 -DUNITY=True\
 -DINWORLD_LOG_CALLBACK=True\
 -DCMAKE_SYSTEM_NAME=Android\
 -DCMAKE_SYSTEM_VERSION=29\
 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a\
 -DCMAKE_ANDROID_NDK=/Users/runner/Library/Android/sdk/ndk/26.3.11579264