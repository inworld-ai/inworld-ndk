cmake_minimum_required (VERSION 3.22)

project(webrtc_aec_plugin VERSION 1.0.0)

set(webrtcDir third-party/webrtc)

if (WIN32)
    set(commonCompileDefines
        RTC_DISABLE_LOGGING
        WEBRTC_WIN
        WIN32
        _WINDOWS
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
elseif (MAC)
    set(commonCompileDefines
        RTC_DISABLE_LOGGING
        WEBRTC_MAC
        WEBRTC_POSIX
    )
    message("architecture: ${CMAKE_OSX_ARCHITECTURES}")
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
        list(APPEND commonCompileDefines WEBRTC_ARCH_ARM_FAMILY)
    elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
        list(APPEND commonCompileDefines WEBRTC_ARCH_X86_FAMILY)
    else()
        message(FATAL "arch unsupported")
    endif()
endif()

# webrtc_base
set(baseHeaders 
    ${webrtcDir} 
    ${webrtcDir}/base 
    ${webrtcDir}/modules
)

# absl
set(abslDir ${webrtcDir}/base/abseil/absl)

file(GLOB_RECURSE abslFiles
     ${abslDir}/strings/*.h
     ${abslDir}/strings/*.cpp
     ${abslDir}/strings/*.cc
     ${abslDir}/strings/*.c
)

list(APPEND abslFiles
    ${abslDir}/types/bad_optional_access.cc
    ${abslDir}/types/bad_optional_access.h

    ${abslDir}/base/internal/throw_delegate.cc
    ${abslDir}/base/internal/throw_delegate.h
)

list(APPEND baseHeaders ${webrtcDir}/base/abseil)

# json
set(jsonDir ${webrtcDir}/base/jsoncpp)

set(jsonFiles
    ${jsonDir}/src/lib_json/json_reader.cpp
    ${jsonDir}/src/lib_json/json_value.cpp
    ${jsonDir}/src/lib_json/json_writer.cpp
)

list(APPEND baseHeaders ${jsonDir}/include)

# rtc_base
file(GLOB_RECURSE rtcBaseFiles
     ${webrtcDir}/base/rtc_base/*.h
     ${webrtcDir}/base/rtc_base/*.cpp
     ${webrtcDir}/base/rtc_base/*.cc
     ${webrtcDir}/base/rtc_base/*.c
)

#system_wrappers
file(GLOB_RECURSE sysWrappersFiles
     ${webrtcDir}/base/system_wrappers/source/*.h
     ${webrtcDir}/base/system_wrappers/source/*.cpp
     ${webrtcDir}/base/system_wrappers/source/*.cc
     ${webrtcDir}/base/system_wrappers/source/*.c
)

list(APPEND baseHeaders ${webrtcDir}/base/system_wrappers/include)

add_library(webrtc_base 
    ${abslFiles}
    ${jsonFiles}
    ${rtcBaseFiles}
    ${sysWrappersFiles}
)
target_compile_features(webrtc_base PUBLIC cxx_std_17)

target_include_directories(webrtc_base PUBLIC
    ${baseHeaders}
)

target_compile_definitions(webrtc_base PUBLIC ${commonCompileDefines})

# webrtc_aec3
file(GLOB_RECURSE FilesAEC3
    ${webrtcDir}/modules/audio_processing/*.h
    ${webrtcDir}/modules/audio_processing/*.cpp
    ${webrtcDir}/modules/audio_processing/*.cc
    ${webrtcDir}/modules/audio_processing/*.c
)

add_library(webrtc_aec3 ${FilesAEC3})
target_compile_features(webrtc_aec3 PUBLIC cxx_std_17)

target_include_directories(webrtc_aec3 PUBLIC
    ${webrtcDir}/modules/audio_processing
    ${baseHeaders}
)

target_compile_definitions(webrtc_aec3 PUBLIC ${commonCompileDefines})

# webrtc_api
file(GLOB_RECURSE apiFiles
     ${webrtcDir}/api/*.h
     ${webrtcDir}/api/*.cpp
     ${webrtcDir}/api/*.cc
     ${webrtcDir}/api/*.c
)

add_library(webrtc_api 
    ${apiFiles}
)
target_compile_features(webrtc_api PUBLIC cxx_std_17)

target_include_directories(webrtc_api PUBLIC
    ${webrtcDir}/webrtc_api
    ${baseHeaders}
)

target_link_libraries(webrtc_api PUBLIC webrtc_base webrtc_aec3)  

target_compile_definitions(webrtc_api PUBLIC ${commonCompileDefines})

# aec plugin
file(GLOB_RECURSE aecPluginFiles
     src/*.h
     src/*.cpp
     src/*.cc
     src/*.c
)

add_library(webrtc_aec_plugin SHARED ${aecPluginFiles})
target_compile_features(webrtc_aec_plugin PUBLIC cxx_std_17)

target_compile_definitions(webrtc_aec_plugin PUBLIC ${commonCompileDefines})

target_include_directories(webrtc_aec_plugin PUBLIC
    src
)

target_link_libraries(webrtc_aec_plugin PUBLIC webrtc_base webrtc_aec3 webrtc_api)  