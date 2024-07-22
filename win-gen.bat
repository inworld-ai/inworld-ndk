mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -DINWORLD_SHARED=False -DAEC=True -DINWORLD_LOG_CALLBACK=True -DINWORLD_AUDIO_DUMP=True

pause