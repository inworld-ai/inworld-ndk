cd ../../..
mkdir build
cd build
cmake --build . --target protoc --config Release
cmake --build . --target grpc_cpp_plugin --config Release

cd ../Example/Unity_NDK/proto
python build-proto.py

pause