sh mac-build-for-arch.sh "x86_64"
sh mac-build-for-arch.sh "arm64"

mkdir $(pwd)/build/Release/universal
lipo -create -output $(pwd)/build/Release/universal/libwebrtc_aec_plugin.dylib $(pwd)/build/Release/x86_64/libwebrtc_aec_plugin.dylib $(pwd)/build/Release/arm64/libwebrtc_aec_plugin.dylib
file $(pwd)/build/Release/universal/libwebrtc_aec_plugin.dylib