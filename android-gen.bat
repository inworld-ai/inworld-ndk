mkdir build
cd build
cmake .. -DAEC=False^
 -DANDROID=True^
 -DPLATFORM=android^
 -DANDROID_PLATFORM=android-30^
 -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android^
 -DANDROID_TOOLCHAIN=clang^
 -DANDROID_ABI=arm64-v8a^
 -DANDROID_STL=c++_static^
 -DCMAKE_TOOLCHAIN_FILE=C:/Users/Legion/AppData/Local/Android/Sdk/ndk/25.2.9519653/build/cmake/android.toolchain.cmake^
 -DANDROID_NDK=C:/Users/Legion/AppData/Local/Android/Sdk/ndk/25.2.9519653

pause


:: -DCMAKE_TOOLCHAIN_FILE=C:/Microsoft/AndroidNDK/android-ndk-r23c/build/cmake/android.toolchain.cmake^
:: -DANDROID_TOOLCHAIN=clang^
:: -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android^
:: -DANDROID_PLATFORM=android-30^
:: -DANDROID_STL=c++_static^
:: -DANDROID_ABI=arm64-v8a^
:: -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a