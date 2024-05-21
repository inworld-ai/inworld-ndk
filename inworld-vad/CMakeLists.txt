cmake_minimum_required (VERSION 3.22)

project(inworld-ndk-vad VERSION 1.0.0)

if(WIN32)
    message("Build VAD")
else()
    message(FATAL_ERROR "VAD: unsupported platform")
endif()

file(GLOB_RECURSE srcFiles
     src/*.h
     src/*.cpp
     src/*.cc
     src/*.c
)
add_library(inworld-ndk-vad SHARED ${srcFiles})
set_target_properties(inworld-ndk-vad PROPERTIES
            LIBRARY_OUTPUT_NAME "inworld-ndk-vad.dll"
            IMPORT_SUFFIX ".dll.lib"
        )
target_compile_features(inworld-ndk-vad PRIVATE cxx_std_20)
target_link_libraries(inworld-ndk-vad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third-party/onnxruntime/lib/onnxruntime.lib)
target_include_directories(inworld-ndk-vad PRIVATE third-party/onnxruntime/include)