cmake_minimum_required (VERSION 3.22)

project(inworld-ndk VERSION 1.2.0)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# inworld-ndk setup
set(ThirdPartyDir ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

file(GLOB_RECURSE FilesALL
    src/*.h
    src/*.cpp
    src/*.cc
    src/*.c
)
file(GLOB_RECURSE FilesALLProto
    src-proto/*.h
    src-proto/*.cpp
    src-proto/*.cc
    src-proto/*.c
)

add_library(inworld-ndk-proto STATIC ${FilesALLProto})
if (INWORLD_SHARED)
    add_library(inworld-ndk SHARED ${FilesALL})
else()
    add_library(inworld-ndk STATIC ${FilesALL})
endif()
target_link_libraries(inworld-ndk PRIVATE inworld-ndk-proto)

target_compile_definitions(inworld-ndk PRIVATE
    NDK_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NDK_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NDK_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    NDK_VERSION_STRING="${PROJECT_VERSION}"
)

if(ANDROID)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
    target_compile_definitions(inworld-ndk PRIVATE ANDROID=1)
endif()

target_include_directories(inworld-ndk PUBLIC
    ${ThirdPartyDir}/include
    ${ThirdPartyDir}/grpc/include
    src/Public
    src/Public/Utils
    src-proto
    src/Private/ThirdParty
)
target_include_directories(inworld-ndk-proto PUBLIC
    ${ThirdPartyDir}/include
    ${ThirdPartyDir}/grpc/include
    src-proto
)

target_compile_features(inworld-ndk PUBLIC cxx_std_20)
target_compile_features(inworld-ndk-proto PUBLIC cxx_std_20)

if (INWORLD_AUDIO_DUMP)
    target_compile_definitions(inworld-ndk PRIVATE INWORLD_AUDIO_DUMP=1)
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
    
    set(protobuf_BUILD_PROTOC_BINARIES OFF CACHE BOOL "Build libprotoc and protoc compiler")
    set(protobuf_BUILD_LIBPROTOC OFF CACHE BOOL "Build libprotoc")
    set(CARES_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)")
endif()

set(CARES_STATIC ON)
set(CARES_STATIC_PIC ON)
set(gRPC_BUILD_GRPC_CPP_PLUGIN ON)
set(gRPC_BUILD_CODEGEN ON)
set(gRPC_BACKWARDS_COMPATIBILITY_MODE ON)

set(CARES_BUILD_TESTS OFF)
set(CARES_BUILD_TOOLS OFF)
set(gRPC_BUILD_CSHARP_EXT OFF)
set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF)
set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF)
set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF)
set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF)
set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF)
set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF)
set(gRPC_BUILD_TOOLS OFF)
set(gRPC_BUILD_TESTS OFF)
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_SHARED_LIBS OFF)

set(gRPC_ABSL_PROVIDER module)
set(gRPC_CARES_PROVIDER module)
set(gRPC_PROTOBUF_PROVIDER module)
set(gRPC_RE2_PROVIDER module)

if (INWORLD_SHARED)
    set(gRPC_ZLIB_PROVIDER module)
    set(gRPC_SSL_PROVIDER module)
else()
    set(gRPC_ZLIB_PROVIDER package)
    set(gRPC_SSL_PROVIDER package)
endif()

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

target_link_libraries(inworld-ndk-proto PUBLIC grpc)
target_link_libraries(inworld-ndk-proto PUBLIC grpc++)

# Unity Setup
option(UNITY FALSE)
if(UNITY)
    message("Building for Unity...")
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_UNITY)
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
    target_include_directories(InworldUnityWrapper PUBLIC inworld-ndk/src/Public)    
    target_link_libraries(InworldUnityWrapper PRIVATE inworld-ndk)  
endif()

# logging
if(INWORLD_LOG_CALLBACK)
	# custom
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_LOG=1)
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_LOG_CALLBACK=1)
endif()

if(INWORLD_LOG_SPD)
    # spdlog
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_LOG=1)
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_LOG_SPD=1)

    if (WIN32)
        set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of fmt library." FORCE)
    else()
        set(SPDLOG_USE_STD_FORMAT OFF CACHE BOOL "Use std::format instead of fmt library." FORCE)
    endif()

    add_subdirectory(ThirdParty/spdlog)

    target_include_directories(inworld-ndk PUBLIC
        ${ThirdPartyDir}/spdlog/include
    )
    target_link_libraries(inworld-ndk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()

if(INWORLD_LOG)
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_LOG=1)
endif()

# gtest
add_subdirectory(ThirdParty/googletest)
target_link_libraries(inworld-ndk PUBLIC gtest)

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(inworld-ndk-unit src/Private/Test/GTest.cpp)
target_link_libraries(inworld-ndk-unit gtest gtest_main)
target_link_libraries(inworld-ndk-unit inworld-ndk)
target_include_directories(inworld-ndk-unit PUBLIC 
    src/Public
)
add_test( inworld-ndk-unit inworld-ndk-unit )
if (WIN32)
    add_custom_command(
        TARGET inworld-ndk-unit POST_BUILD
        COMMAND inworld-ndk-unit
        VERBATIM
    )
endif()

# post build package
set(PackageDir ${PROJECT_SOURCE_DIR}/build/Package)

add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir})
add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/include)
add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/src)
add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/lib)

if (WIN32)
    set(PackageLibDir  ${PackageDir}/lib/Win64)
    set(LibExt lib)
    set(DynamicLibExt dll)
    set(LibPref "")
elseif (ANDROID)
    set(PackageLibDir  ${PackageDir}/lib/Android/${CMAKE_ANDROID_ARCH_ABI})
    set(LibExt a)
    set(DynamicLibExt so)
    set(LibPref lib)
elseif (IOS)
    set(PackageLibDir  ${PackageDir}/lib/iOS)
    set(LibExt a)
    set(DynamicLibExt dylib)
    set(LibPref lib)
elseif (APPLE)
    set(PackageLibDir  ${PackageDir}/lib/Mac)
    set(LibExt a)
    set(DynamicLibExt dylib)
    set(LibPref lib)
endif()

add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PackageLibDir})

set(GrpcDir ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc)
set(AbslDir ${GrpcDir}/third_party/abseil-cpp/absl)

if (IOS)
    set(ConfigDirName /$<CONFIGURATION>-iphoneos)
    set(ConfigDirNameCares /$<CONFIGURATION>)
elseif (ANDROID)
    set(ConfigDirName "")
    set(ConfigDirNameCares "")
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

if (INWORLD_SHARED)
    set(LibFiles 
        ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk.${DynamicLibExt}
    )
    if (WIN32)
        list(APPEND LibFiles
            ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk.${LibExt})
    endif()
else()
    set(LibFiles 
        ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk.${LibExt}
        ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk-proto.${LibExt}
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
endif()

if (INWORLD_LOG_SPD AND NOT INWORLD_SHARED)
    list(APPEND LibFiles
        ${CMAKE_CURRENT_SOURCE_DIR}/build/ThirdParty/spdlog${ConfigDirName}/${LibPref}${LibSpglogName}.${LibExt})
endif()

# webrtc aec setup
if (AEC)
    if (WIN32 OR MAC)
        target_compile_definitions(inworld-ndk PUBLIC INWORLD_AEC)
        target_include_directories(inworld-ndk PUBLIC inworld-aec/src)
        add_custom_command(TARGET inworld-ndk POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/src/AECInterop.h  ${PackageDir}/include)
    else()
        message(FATAL_ERROR "AEC: not supported")
    endif()
    
    if (WIN32)
        target_link_libraries(inworld-ndk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.lib)

        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.lib
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.dll
        )
    elseif (MAC)
        target_link_libraries(inworld-ndk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Mac/libwebrtc_aec_plugin.dylib)

        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Mac/libwebrtc_aec_plugin.dylib
        )
    endif()
endif()

foreach(LibFile IN LISTS LibFiles)
    add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LibFile} ${PackageLibDir})
    message("Copy lib to package: ${LibFile}")
endforeach()

function(CopyIncludeDir Path)
    add_custom_command(TARGET inworld-ndk POST_BUILD
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

add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/src  ${PackageDir}/src)

add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/src-proto  ${PackageDir}/src-proto)

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
GroupSourcesByFolder(inworld-ndk)
GroupSourcesByFolder(inworld-ndk-proto)

# setup example
if (WIN32 OR MAC)
    set(SourceDir Example/ConsoleApp/src)

    file(GLOB_RECURSE FilesALL
        "${SourceDir}/*.h"
        "${SourceDir}/*.cpp"
        "${SourceDir}/*.cc"
        "${SourceDir}/*.c"
    )

    add_executable(inworld-ndk-app ${FilesALL})

    target_compile_features(inworld-ndk-app PRIVATE cxx_std_20)
    target_include_directories(inworld-ndk-app PUBLIC inworld-ndk/src/Public)
    target_link_libraries(inworld-ndk-app PRIVATE inworld-ndk)

    GroupSourcesByFolder(inworld-ndk-app)

    if (AEC AND WIN32)
        add_custom_command(TARGET inworld-ndk-app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.dll  ${PROJECT_SOURCE_DIR}/build${ConfigDirName})
        if (INWORLD_SHARED)
            add_custom_command(TARGET inworld-ndk-app POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk.${DynamicLibExt}  ${PROJECT_SOURCE_DIR}/build${ConfigDirName})
        endif()
    endif()
endif()
