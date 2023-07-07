mkdir build
cd build
cmake --build . --target protoc --config Release
cmake --build . --target grpc_cpp_plugin --config Release

cd ..
python build-proto.py

pause