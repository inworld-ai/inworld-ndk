cmake_minimum_required (VERSION 3.22)

if (LINUX)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif()

project(inworld-ndk VERSION 1.4.1.rc-1)

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
set_target_properties(inworld-ndk-proto PROPERTIES DEBUG_POSTFIX "")
if (INWORLD_SHARED)
    add_library(inworld-ndk SHARED ${FilesALL})
    if (WINDOWS OR XSX)
        set_target_properties(inworld-ndk PROPERTIES
            LIBRARY_OUTPUT_NAME "inworld-ndk.dll"
            IMPORT_SUFFIX ".dll.lib"
        )
    endif()
else()
    add_library(inworld-ndk STATIC ${FilesALL})
endif()
set_target_properties(inworld-ndk PROPERTIES DEBUG_POSTFIX "")

if (MSVC)
    target_compile_options(inworld-ndk PRIVATE $<$<CONFIG:RelWithDebInfo>:/Od>)
else()
    target_compile_options(inworld-ndk PRIVATE $<$<CONFIG:RelWithDebInfo>:-O0>)
endif()

target_link_libraries(inworld-ndk PRIVATE inworld-ndk-proto)

target_compile_definitions(inworld-ndk PRIVATE
    NDK_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NDK_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NDK_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    NDK_VERSION_STRING="${PROJECT_VERSION}"
)

if(LINUX)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
	add_definitions(-nostdinc++)
	add_definitions(-I${ThirdPartyDir}/LibCxx/include)
	add_definitions(-I${ThirdPartyDir}/LibCxx/include/c++/v1)
	link_libraries("-nodefaultlibs")
	link_libraries("${ThirdPartyDir}/LibCxx/lib/Unix/x86_64-unknown-linux-gnu/libc++.a")
	link_libraries("${ThirdPartyDir}/LibCxx/lib/Unix/x86_64-unknown-linux-gnu/libc++abi.a")
	link_libraries("-lm")
	link_libraries("-lc")
	link_libraries("-lgcc_s")
	link_libraries("-lgcc")
endif()

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
    if (WINDOWS OR MAC)
        target_compile_definitions(inworld-ndk PRIVATE INWORLD_AUDIO_DUMP=1)
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

# xbox cares patch 
if (XSX)
	set(caresDir ${ThirdPartyDir}/grpc/third_party/cares)
	configure_file(${caresDir}/patch/ares_strcasecmp.c ${caresDir}/cares/ares_strcasecmp.c)
	configure_file(${caresDir}/patch/ares_strdup.c ${caresDir}/cares/ares_strdup.c)
	configure_file(${caresDir}/patch/ares_strsplit.c ${caresDir}/cares/ares_strsplit.c)
endif()

if (IOS OR ANDROID OR LINUX)
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

if (INWORLD_SHARED AND IOS)
    set(gRPC_ZLIB_PROVIDER "module" CACHE STRING "Provider of zlib library")
    set(gRPC_SSL_PROVIDER "module" CACHE STRING "Provider of ssl library")
else()
    set(gRPC_ZLIB_PROVIDER "package" CACHE STRING "Provider of zlib library")
    set(gRPC_SSL_PROVIDER "package" CACHE STRING "Provider of ssl library")
endif()

if (WINDOWS)
    set(ZlibPlatformSubpath lib/Win64/Release/zlibstatic.lib)
    set(LibcryptoPlatformSubpath Lib/Win64/VS2015/Release/libcrypto.lib)
    set(LibsslPlatformSubpath Lib/Win64/VS2015/Release/libssl.lib)
    set(OpensslIncludeSubpath include/Win64/VS2015)
elseif (XSX)
    set(ZlibPlatformSubpath lib/XSX/zlibstatic.lib)
    set(LibcryptoPlatformSubpath Lib/XSX/libcrypto.lib)
    set(LibsslPlatformSubpath Lib/XSX/libssl.lib)
    set(OpensslIncludeSubpath include/XSX)
elseif (ANDROID)
    set(OpensslIncludeSubpath include/Android)
    if (CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
        set(ZlibPlatformSubpath lib/Android/arm-linux-androideabi/libz.a)
        set(LibcryptoPlatformSubpath Lib/Android/arm/libcrypto.a)
        set(LibsslPlatformSubpath Lib/Android/arm/libssl.a)
    elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
        set(ZlibPlatformSubpath lib/Android/aarch64-linux-android/libz.a)
        set(LibcryptoPlatformSubpath Lib/Android/arm64/libcrypto.a)
        set(LibsslPlatformSubpath Lib/Android/arm64/libssl.a)
    endif()
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
elseif(LINUX)
    if (CMAKE_LINUX_ARCH_ABI STREQUAL "aarch64")
        set(ZlibPlatformSubpath lib/Unix/aarch64-unknown-linux-gnu/Release/libz.a)
        set(LibcryptoPlatformSubpath lib/Unix/aarch64-unknown-linux-gnu/libcrypto.a)
        set(LibsslPlatformSubpath lib/Unix/aarch64-unknown-linux-gnu/libssl.a)
        set(OpensslIncludeSubpath include/Unix/aarch64-unknown-linux-gnu)
    elseif (CMAKE_LINUX_ARCH_ABI STREQUAL "x86_64")
        set(ZlibPlatformSubpath lib/Unix/x86_64-unknown-linux-gnu/Release/libz.a)
        set(LibcryptoPlatformSubpath lib/Unix/x86_64-unknown-linux-gnu/libcrypto.a)
        set(LibsslPlatformSubpath lib/Unix/x86_64-unknown-linux-gnu/libssl.a)
        set(OpensslIncludeSubpath include/Unix/x86_64-unknown-linux-gnu)
    endif()
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
    target_include_directories(grpc PUBLIC
        ${ThirdPartyDir}/zlib/1.2.12/include
    )
    target_include_directories(grpc++ PUBLIC
        ${ThirdPartyDir}/zlib/1.2.12/include
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

    if (WINDOWS)
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
if(WINDOWS OR MAC)
    target_compile_definitions(inworld-ndk PUBLIC INWORLD_WITH_GTEST=1)
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
    if (WINDOWS)
        add_custom_command(
            TARGET inworld-ndk-unit POST_BUILD
            COMMAND inworld-ndk-unit
            VERBATIM
        )
    endif()
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
add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory  ${PackageDir}/resource)

if (WINDOWS)
    set(PackageLibDir  ${PackageDir}/lib/Win64)
    set(LibExt lib)
    set(DynamicLibExt dll)
    set(LibPref "")
elseif (XSX)
    set(PackageLibDir  ${PackageDir}/lib/XSX)
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
elseif (LINUX)
    set(PackageLibDir  ${PackageDir}/lib/Linux/${CMAKE_LINUX_ARCH_ABI})
    set(LibExt a)
    set(DynamicLibExt so)
    set(LibPref lib)
endif()

add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PackageLibDir})

set(GrpcDir ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc)
set(AbslDir ${GrpcDir}/third_party/abseil-cpp/absl)

if (IOS)
    set(ConfigDirName /$<CONFIGURATION>-iphoneos)
    set(ConfigDirNameCares /$<CONFIGURATION>)
elseif (ANDROID OR LINUX)
    set(ConfigDirName "")
    set(ConfigDirNameCares "")
else()
    set(ConfigDirName /$<CONFIGURATION>)
    set(ConfigDirNameCares /$<CONFIGURATION>)
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(INWORLD_DEBUG True)
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
    if (WINDOWS OR XSX)
        list(APPEND LibFiles
            ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/inworld-ndk.dll.lib)
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
    if (WINDOWS OR MAC)
        target_compile_definitions(inworld-ndk PUBLIC INWORLD_AEC)
        target_include_directories(inworld-ndk PUBLIC inworld-aec/src)
        add_custom_command(TARGET inworld-ndk POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/src/AECInterop.h  ${PackageDir}/include)
    else()
        message(FATAL_ERROR "AEC: not supported")
    endif()
    
    if (WINDOWS)
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

# VAD
if (WINDOWS OR MAC)
    if (WINDOWS)
        target_link_libraries(inworld-ndk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Win64/inworld-ndk-vad.dll.lib)
        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Win64/inworld-ndk-vad.dll.lib
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Win64/${LibPref}inworld-ndk-vad.${DynamicLibExt}
        )
    else()
        target_link_libraries(inworld-ndk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Mac/libinworld-ndk-vad.dylib)
        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Mac/${LibPref}inworld-ndk-vad.${DynamicLibExt}
        )
    endif()
    target_include_directories(inworld-ndk PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/src)
    add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/src/InworldVAD.h  ${PackageDir}/include)
    add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/resource/silero_vad_10_27_2022.onnx  ${PackageDir}/resource)
    target_compile_definitions(inworld-ndk PRIVATE INWORLD_VAD)
endif()

foreach(LibFile IN LISTS LibFiles)
    add_custom_command(TARGET inworld-ndk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LibFile} ${PackageLibDir})
    message("Copy lib to package: ${LibFile}")
endforeach()

add_custom_command(TARGET inworld-ndk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/src  ${PackageDir}/src)

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
if (WINDOWS OR MAC)
    set(SourceDir Example/ConsoleApp/src)

    file(GLOB_RECURSE FilesALL
        "${SourceDir}/*.h"
        "${SourceDir}/*.cpp"
        "${SourceDir}/*.cc"
        "${SourceDir}/*.c"
    )

    add_executable(inworld-ndk-app ${FilesALL})

    target_compile_features(inworld-ndk-app PRIVATE cxx_std_20)
    target_include_directories(inworld-ndk-app PRIVATE inworld-ndk/src/Public)
    target_link_libraries(inworld-ndk-app PRIVATE inworld-ndk)
    if (MSVC)
        target_compile_options(inworld-ndk-app PRIVATE $<$<CONFIG:RelWithDebInfo>:/Od>)
    else()
        target_compile_options(inworld-ndk-app PRIVATE $<$<CONFIG:RelWithDebInfo>:-O0>)
    endif()

    GroupSourcesByFolder(inworld-ndk-app)

    if (INWORLD_SHARED)
        add_custom_command(TARGET inworld-ndk-app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}inworld-ndk.${DynamicLibExt}  ${PROJECT_SOURCE_DIR}/build${ConfigDirName})
    endif()
            
    if (AEC AND WINDOWS)
        add_custom_command(TARGET inworld-ndk-app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.dll  ${PROJECT_SOURCE_DIR}/build${ConfigDirName})
    endif()
    
    if (WINDOWS)
        target_include_directories(inworld-ndk-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/src)
        target_link_libraries(inworld-ndk-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Win64/inworld-ndk-vad.dll.lib)
        add_custom_command(TARGET inworld-ndk-app POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Win64/inworld-ndk-vad.dll  ${PROJECT_SOURCE_DIR}/build${ConfigDirName})
    elseif(MAC)
        target_include_directories(inworld-ndk-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/src)
        target_link_libraries(inworld-ndk-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/prebuilt/Mac/libinworld-ndk-vad.dylib)
    endif()

    if (WINDOWS OR MAC)
        if (WINDOWS)
            option(RTAUDIO_API_DS "Build DirectSound API" ON)
            option(RTAUDIO_API_WASAPI "Build WASAPI API" OFF)
        endif()
        add_subdirectory(ThirdParty/rtaudio)
        target_link_libraries(inworld-ndk-app PRIVATE rtaudio)
        target_include_directories(inworld-ndk-app PRIVATE ThirdParty/rtaudio)
    endif()
endif()

# cppcheck
if(WINDOWS AND INWORLD_CPPCHECK)
    message("configuring CppCheck")

    set(CMD_CPPCHECK "C:/Program Files/Cppcheck/cppcheck.exe")
    if(NOT EXISTS ${CMD_CPPCHECK})
        message(FATAL_ERROR "couldn't find cppcheck")
    endif()

    include(ProcessorCount)
    ProcessorCount(CPU_CORES)
    
    function(AddCppCheck Target Source)
        list(
        APPEND CMD_CPPCHECK
            "-j ${CPU_CORES}"
            "--quiet"
            "--enable=style"
            "--std=c++20"
            "--platform=win64"
            "-iPrivate/ThirdParty"
            "--inline-suppr"
            "--suppressions-list=${CMAKE_CURRENT_SOURCE_DIR}/cppcheck_suppressions.txt"
            "--template=\"${Source}/{file}({line}): cppcheck fail: {severity} ({id}): {message}\""
            "."
        )

        add_custom_target(${Target}-cppcheck
            COMMAND ${CMD_CPPCHECK}
            WORKING_DIRECTORY ${Source}
            COMMENT "${Target} static code analysis"
        )
        add_custom_command(TARGET ${Target} POST_BUILD
            COMMAND ${CMD_CPPCHECK}
            WORKING_DIRECTORY ${Source}
            COMMENT "${Target} static code analysis"
        )
    endfunction()

    AddCppCheck(inworld-ndk ${CMAKE_CURRENT_SOURCE_DIR}/src)
    AddCppCheck(inworld-ndk-app ${CMAKE_CURRENT_SOURCE_DIR}/Example/ConsoleApp/src)
    #AddCppCheck(inworld-ndk-vad ${CMAKE_CURRENT_SOURCE_DIR}/inworld-vad/src)
endif()
