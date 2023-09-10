mkdir build
cd build
set curpath=%cd%
cmake .. -DAEC=True -DINWORLD_LOG_SPD=True -DSTT_LOCAL=True -DWITH_MKL=False -DWITH_DNNL=True^
 -DDNNL_INCLUDE_DIR=%curpath%/ThirdParty/oneDNN/build/include^
 -DDNNL_LIBRARY=%curpath%/ThirdParty/oneDNN/build/src/Release

pause