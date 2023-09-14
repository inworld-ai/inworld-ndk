mkdir build
cd build
::set rootpath=%cd%/..
cmake .. -DAEC=True -DINWORLD_LOG_SPD=True -DSTT_LOCAL="WHISPERCPP" -DWHISPER_SDL2=True -DWHISPER_BUILD_EXAMPLES=True
:: -DWITH_MKL=False -DWITH_DNNL=True -DOPENMP_RUNTIME="COMP"^
:: -DDNNL_INCLUDE_DIR=%rootpath%/ThirdParty/oneDNN/include^
:: -DDNNL_LIBRARY=%rootpath%/ThirdParty/oneDNN/build/src/Release

pause