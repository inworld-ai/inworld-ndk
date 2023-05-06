cmake_minimum_required (VERSION 3.23)

project(InworldNDKApp)

add_subdirectory(InworldNDK)

set(SourceDir src)

file(GLOB_RECURSE FilesALL
    "${SourceDir}/*.h"
    "${SourceDir}/*.cpp"
    "${SourceDir}/*.cc"
    "${SourceDir}/*.c"
)

add_executable(InworldNDKApp ${FilesALL})

target_compile_features(InworldNDKApp PRIVATE cxx_std_20)
target_include_directories(InworldNDKApp PUBLIC InworldNDK/src)
target_link_libraries(InworldNDKApp PRIVATE InworldNDK)

if (AEC)
    if (Win32)
        add_custom_command(TARGET InworldNDKApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/InworldNDK/ThirdParty/webrtc-aec/webrtc-checkout/src/out/webrtc_aec_plugin.dll ${PROJECT_SOURCE_DIR}/build/$<CONFIGURATION>)
    endif()
endif()

