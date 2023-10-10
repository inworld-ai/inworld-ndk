mkdir build
cd build
cmake .. -DAEC=False -DANDROID=True -DUNITY=True -DINWORLD_LOG_CALLBACK=True -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=31 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_ANDROID_NDK="C:\Program Files\Unity\Hub\Editor\2022.3.10f1\Editor\Data\PlaybackEngines\AndroidPlayer\NDK"