cd thirdparty/onednn
mkdir build
cd build
cmake ..
cmake --build . --config=Release
cmake --build . --target install --config=Release