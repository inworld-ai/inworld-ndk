architecture=$1

sh mac-gen.sh "$architecture"
sh mac-build.sh
file $(pwd)/build/Release/libwebrtc_aec_plugin.dylib
mkdir $(pwd)/build/Release/$architecture
mv $(pwd)/build/Release/libwebrtc_aec_plugin.dylib $(pwd)/build/Release/$architecture/libwebrtc_aec_plugin.dylib