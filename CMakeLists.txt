cmake_minimum_required (VERSION 3.22)
option(UNITY FALSE)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

function(GroupSourcesByFolder target)
    set(SOURCE_GROUP_DELIMITER "/")
    set(last_dir "")
    set(files "")

    get_target_property(sources ${target} SOURCES)

    foreach(file ${sources})
    file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}" ${file})
    get_filename_component(dir "${relative_file}" PATH)
    if(NOT "${dir}" STREQUAL "${last_dir}")
        if(files)
            source_group("${last_dir}" FILES ${files})
        endif()
        set(files "")
    endif()
    set(files ${files} ${file})
    set(last_dir "${dir}")
    endforeach()

    if(files)
        source_group("${last_dir}" FILES ${files})
    endif()
endfunction()

find_package(Git REQUIRED)

# update dependecies
if(UPDATE_DEPENDENCIES)
    message("Updating dependencies...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        RESULT_VARIABLE error_code
    )
    if(error_code)
        message(FATAL_ERROR "Failed to update dependecies")
    else()
        message("Dependecies updated")
    endif()
endif()

# InworldNDK setup
project(InworldNDK)

set(ThirdPartyDir ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

file(GLOB_RECURSE FilesALL
    src/*.h
    src/*.cpp
    src/*.cc
    src/*.c
)

add_library(InworldNDK ${FilesALL})

if (ANDROID)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
    target_compile_definitions(InworldNDK PRIVATE ANDROID=1)
endif()

target_include_directories(InworldNDK PUBLIC
    ${ThirdPartyDir}/include
    ${ThirdPartyDir}/grpc/include
    src
    src/proto
    src/ThirdParty
)

if (ANDROID)
    #target_compile_options(InworldNDK PRIVATE -std=c++1z)
    #set_target_properties(c PROPERTIES
    #    CXX_STANDARD 17
    #    CXX_STANDARD_REQUIRED ON
    #    CXX_EXTENSIONS OFF
    #)
    #set_target_properties(InworldNDK PROPERTIES CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    set_property(TARGET InworldNDK PROPERTY CMAKE_CXX_STANDARD 17)
else()
    target_compile_features(InworldNDK PRIVATE cxx_std_20)
endif()

GroupSourcesByFolder(InworldNDK)

# webrtc aec setup
if (AEC)
    target_compile_definitions(InworldNDK PUBLIC INWORLD_AEC)
    target_include_directories(InworldNDK PUBLIC aec/aecplugin)
    if (WIN32)
        target_link_libraries(InworldNDK PUBLIC ${ThirdPartyDir}/Prebuilt/Win64/webrtc_aec_plugin.dll.lib)
        add_custom_command(TARGET InworldNDK POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ThirdPartyDir}/Prebuilt/Win64/webrtc_aec_plugin.dll
            ${CMAKE_CURRENT_SOURCE_DIR}/build)
    elseif (MAC)
        target_link_libraries(InworldNDK PUBLIC ${ThirdPartyDir}/Prebuilt/Mac/libwebrtc_aec_plugin.dylib)
    else()
        message(FATAL_ERROR "AEC: not supported")
    endif()
endif()

# grpc setup
if(UE_DIR)
    set(ZlibDir ${UE_DIR}/Engine/Source/ThirdParty/zlib/1.2.12)
    set(OpenSSLDir ${UE_DIR}/Engine/Source/ThirdParty/OpenSSL/1.1.1n)
else()
    set(ZlibDir ${ThirdPartyDir}/zlib/1.2.12)
    set(OpenSSLDir ${ThirdPartyDir}/OpenSSL/1.1.1n)
endif()

function(CheckPathValid Path)
    if (NOT EXISTS ${Path})
        message(FATAL_ERROR "Invalid path ${Path}")
    endif()
    message("Valid path ${Path}")
endfunction()

CheckPathValid(${ZlibDir})
CheckPathValid(${OpenSSLDir})

# Unity Setup
if(UNITY)
    message("Building for Unity...")
    target_compile_definitions(InworldNDK PUBLIC INWORLD_UNITY)
    file(GLOB_RECURSE UnityNDKFiles
        Example/Unity/*.h
        Example/Unity/*.cpp
        Example/Unity/*.cc
        Example/Unity/*.c
    )        
    list(APPEND FilesALL ${UnityNDKFiles})
    if (IOS)
        add_library(InworldUnityWrapper STATIC ${UnityNDKFiles})
    else()
        add_library(InworldUnityWrapper SHARED ${UnityNDKFiles})
    endif()
    target_include_directories(InworldUnityWrapper PUBLIC InworldNDK/src)    
    target_link_libraries(InworldUnityWrapper PRIVATE InworldNDK)  
endif()

if (IOS OR ANDROID)
    set(CMAKE_MAKE_PROGRAM /usr/bin/make)
    set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
    set(HAVE_LIBNSL FALSE)
    set(HAVE_SOCKET_LIBSOCKET FALSE)
    set(HAVE_GHBN_LIBSOCKET FALSE)
    set(HAVE_LIBSOCKET FALSE)
    set(HAVE_LIBRT FALSE)
    set(ENABLE_BITCODE ON)
    
    set(protobuf_BUILD_PROTOC_BINARIES OFF CACHE BOOL "Build libprotoc and protoc compiler")
    set(protobuf_BUILD_LIBPROTOC OFF CACHE BOOL "Build libprotoc")
    set(CARES_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)")
endif()

set(CARES_STATIC ON)
set(CARES_STATIC_PIC ON)
set(CARES_BUILD_TESTS OFF)
set(CARES_BUILD_TOOLS OFF)
set(gRPC_BUILD_CSHARP_EXT OFF)
set(gRPC_BUILD_GRPC_CPP_PLUGIN OFF)
set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF)
set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF)
set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF)
set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF)
set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF)
set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF)
set(gRPC_BUILD_CODEGEN OFF)
set(gRPC_BUILD_TOOLS OFF)
set(gRPC_BACKWARDS_COMPATIBILITY_MODE ON)
set(gRPC_BUILD_TESTS OFF)
set(gRPC_ABSL_PROVIDER module)
set(gRPC_CARES_PROVIDER module)
set(gRPC_PROTOBUF_PROVIDER module)
set(gRPC_RE2_PROVIDER module)
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_SHARED_LIBS OFF)

set(gRPC_ZLIB_PROVIDER "package" CACHE STRING "Provider of zlib library")
set(gRPC_SSL_PROVIDER "package" CACHE STRING "Provider of ssl library")

if (WIN32)
    set(ZlibPlatformSubpath lib/Win64/Release/zlibstatic.lib)
    set(LibcryptoPlatformSubpath Lib/Win64/VS2015/Release/libcrypto.lib)
    set(LibsslPlatformSubpath Lib/Win64/VS2015/Release/libssl.lib)
    set(OpensslIncludeSubpath include/Win64/VS2015)
elseif (ANDROID)
    set(ZlibPlatformSubpath lib/Android/aarch64-linux-android/libz.a)
    set(LibcryptoPlatformSubpath Lib/Android/arm64/libcrypto.a)
    set(LibsslPlatformSubpath Lib/Android/arm64/libssl.a)
    set(OpensslIncludeSubpath include/Android)
elseif (IOS)
    set(ZlibPlatformSubpath lib/iOS/libz.a)
    set(LibcryptoPlatformSubpath Lib/IOS/libcrypto.a)
    set(LibsslPlatformSubpath Lib/IOS/libssl.a)
    set(OpensslIncludeSubpath include/IOS)
elseif (APPLE)
    set(ZlibPlatformSubpath lib/Mac/Release/libz.a)
    set(LibcryptoPlatformSubpath Lib/Mac/libcrypto.a)
    set(LibsslPlatformSubpath Lib/Mac/libssl.a)
    set(OpensslIncludeSubpath include/Mac)
endif()

set(ZLIB_INCLUDE_DIR ${ZlibDir}/include)
set(ZLIB_LIBRARY_DEBUG ${ZlibDir}/${ZlibPlatformSubpath})
set(ZLIB_LIBRARY_RELEASE ${ZLIB_LIBRARY_DEBUG})
set(OPENSSL_CRYPTO_LIBRARY ${OpenSSLDir}/${LibcryptoPlatformSubpath})
set(LIB_EAY_LIBRARY_DEBUG ${OPENSSL_CRYPTO_LIBRARY})
set(LIB_EAY_LIBRARY_RELEASE ${OPENSSL_CRYPTO_LIBRARY})
set(LIB_EAY_DEBUG ${OPENSSL_CRYPTO_LIBRARY})
set(LIB_EAY_RELEASE ${OPENSSL_CRYPTO_LIBRARY})

set(OPENSSL_INCLUDE_DIR ${OpenSSLDir}/${OpensslIncludeSubpath})
set(OPENSSL_ROOT_DIR ${OpenSSLDir})
set(OPENSSL_SSL_LIBRARY ${OpenSSLDir}/${LibsslPlatformSubpath})
set(SSL_EAY_LIBRARY_DEBUG ${OPENSSL_SSL_LIBRARY})
set(SSL_EAY_LIBRARY_RELEASE ${OPENSSL_SSL_LIBRARY})
set(SSL_EAY_DEBUG ${OPENSSL_SSL_LIBRARY})
set(SSL_EAY_RELEASE ${OPENSSL_SSL_LIBRARY})

CheckPathValid(${ZLIB_INCLUDE_DIR})
CheckPathValid(${ZLIB_LIBRARY_RELEASE})
CheckPathValid(${OPENSSL_CRYPTO_LIBRARY})
CheckPathValid(${OPENSSL_ROOT_DIR})
CheckPathValid(${OPENSSL_SSL_LIBRARY})

add_subdirectory(ThirdParty/grpc)

if (ANDROID)
    target_include_directories(grpc PUBLIC
        ${ThirdPartyDir}/OpenSSL/1.1.1n/include/Android
    )
    target_include_directories(grpc++ PUBLIC
        ${ThirdPartyDir}/OpenSSL/1.1.1n/include/Android
    )
endif()

target_link_libraries(InworldNDK PUBLIC grpc)
target_link_libraries(InworldNDK PUBLIC grpc++)

# logging
if(INWORLD_LOG_CALLBACK)
	# custom
    target_compile_definitions(InworldNDK PUBLIC INWORLD_LOG=1)
    target_compile_definitions(InworldNDK PUBLIC INWORLD_LOG_CALLBACK=1)
endif()

if(INWORLD_LOG_SPD)
    # spdlog
    target_compile_definitions(InworldNDK PUBLIC INWORLD_LOG=1)
    target_compile_definitions(InworldNDK PUBLIC INWORLD_LOG_SPD=1)

    if (WIN32)
        set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of fmt library." FORCE)
    else()
        set(SPDLOG_USE_STD_FORMAT OFF CACHE BOOL "Use std::format instead of fmt library." FORCE)
    endif()

    add_subdirectory(ThirdParty/spdlog)

    target_include_directories(InworldNDK PUBLIC
        ${ThirdPartyDir}/spdlog/include
    )
    target_link_libraries(InworldNDK PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()

if(INWORLD_LOG)
    target_compile_definitions(InworldNDK PUBLIC INWORLD_LOG=1)
endif()

# gtest
if (WIN32 OR MAC)
    add_subdirectory(ThirdParty/googletest)
    target_link_libraries(InworldNDK PUBLIC gtest)

    enable_testing()
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    add_executable( InworldNDKUnit src/Test/GTest.cpp )
    target_link_libraries(InworldNDKUnit gtest gtest_main)
    target_link_libraries(InworldNDKUnit InworldNDK)
    target_include_directories(InworldNDKUnit PUBLIC 
        src
    )
    add_test( InworldNDKUnit InworldNDKUnit )
    if (WIN32)
        add_custom_command(
            TARGET InworldNDKUnit POST_BUILD
            COMMAND InworldNDKUnit
            VERBATIM
        )
    endif()
endif()

# post build package
set(PackageDir ${PROJECT_SOURCE_DIR}/build/Package)

add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir})
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/include)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/src)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/lib)

if (WIN32)
    set(PackageLibDir  ${PackageDir}/lib/Win64)
    set(LibExt lib)
    set(LibPref "")
elseif (ANDROID)
    set(PackageLibDir  ${PackageDir}/lib/Android)
    set(LibExt a)
    set(LibPref lib)
elseif (IOS)
    set(PackageLibDir  ${PackageDir}/lib/iOS)
    set(LibExt a)
    set(LibPref lib)
elseif (APPLE)
    set(PackageLibDir  ${PackageDir}/lib/Mac)
    set(LibExt a)
    set(LibPref lib)
endif()

add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PackageLibDir})

set(GrpcDir ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc)
set(AbslDir ${GrpcDir}/third_party/abseil-cpp/absl)

if (IOS)
    set(ConfigDirName /$<CONFIGURATION>-iphoneos)
    set(ConfigDirNameCares /$<CONFIGURATION>)
else()
    set(ConfigDirName /$<CONFIGURATION>)
    set(ConfigDirNameCares /$<CONFIGURATION>)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LibProtobufName libprotobufd)
    if(INWORLD_LOG_SPD)
        set(LibSpglogName spdlogd)
    endif()
else()
    set(LibProtobufName libprotobuf)
    if(INWORLD_LOG_SPD)
        set(LibSpglogName spdlog)
    endif()
endif()

set(LibFiles 
    ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}InworldNDK.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_base.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_malloc_internal.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_raw_logging_internal.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_spinlock_wait.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_throw_delegate.${LibExt}
    ${AbslDir}/types${ConfigDirName}/${LibPref}absl_bad_optional_access.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cord.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_str_format_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings_internal.${LibExt}
    ${AbslDir}/debugging${ConfigDirName}/${LibPref}absl_symbolize.${LibExt}
    ${AbslDir}/debugging${ConfigDirName}/${LibPref}absl_stacktrace.${LibExt}
    ${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_graphcycles_internal.${LibExt}
    ${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_synchronization.${LibExt}
    ${AbslDir}/numeric${ConfigDirName}/${LibPref}absl_int128.${LibExt}
    ${AbslDir}/status${ConfigDirName}/${LibPref}absl_status.${LibExt}
    ${AbslDir}/status${ConfigDirName}/${LibPref}absl_statusor.${LibExt}
    ${AbslDir}/time${ConfigDirName}/${LibPref}absl_time.${LibExt}
    ${AbslDir}/time${ConfigDirName}/${LibPref}absl_time_zone.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}address_sorting.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}gpr.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}grpc.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}grpc++.${LibExt}
    ${GrpcDir}/third_party/cares/cares/lib${ConfigDirNameCares}/${LibPref}cares.${LibExt}
    ${GrpcDir}/third_party/protobuf${ConfigDirName}/${LibProtobufName}.${LibExt}
    ${GrpcDir}/third_party/re2${ConfigDirName}/${LibPref}re2.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}upb.${LibExt}
    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${ZLIB_LIBRARY_RELEASE}
)

if (ANDROID AND INWORLD_LOG_SPD)
    list(APPEND LibFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/build/ThirdParty/spdlog/${LibPref}${LibSpglogName}.${LibExt})
endif()

if (AEC)
    if (WIN32)
        list(APPEND LibFiles 
            ${ThirdPartyDir}/Prebuilt/Win64/webrtc_aec_plugin.dll.lib
            ${ThirdPartyDir}/Prebuilt/Win64/webrtc_aec_plugin.dll
    )
    elseif (MAC)
        list(APPEND LibFiles 
            ${ThirdPartyDir}/Prebuilt/Mac/libwebrtc_aec_plugin.dylib
        )
    endif()
endif()

foreach(LibFile IN LISTS LibFiles)
    add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LibFile} ${PackageLibDir})
    message("Copy lib to package: ${LibFile}")
endforeach()

function(CopyIncludeDir Path)
    add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${Path}  ${PackageDir}/include)
    message("Copy include dir to package: ${Path}")
endfunction()

CopyIncludeDir(${ThirdPartyDir}/include)
CopyIncludeDir(${ThirdPartyDir}/grpc/include)
if(INWORLD_LOG_SPD)
    CopyIncludeDir(${ThirdPartyDir}/spdlog/include)
endif()
CopyIncludeDir(${OPENSSL_INCLUDE_DIR})
CopyIncludeDir(${ZLIB_INCLUDE_DIR})

add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/src  ${PackageDir}/src)

# setup example
if (NOT IOS AND NOT ANDROID)
    set(SourceDir Example/ConsoleApp/src)

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

    if (AEC AND WIN32)
        add_custom_command(TARGET InworldNDKApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${PROJECT_SOURCE_DIR}/ThirdParty/Prebuilt/Win64/webrtc_aec_plugin.dll  ${PackageDir}${ConfigDirName})
    endif()
endif()
