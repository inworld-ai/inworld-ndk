set UE_DIR=C:/Program Files/Epic Games/UE_5.1

mkdir build
cd build
cmake .. -DAEC=True -DUE_DIR="%UE_DIR%"

pause