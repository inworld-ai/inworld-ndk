cmake_minimum_required (VERSION 3.22)

project(InworldNDK VERSION 1.2.0)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# InworldNDK setup
set(ThirdPartyDir ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

file(GLOB_RECURSE FilesALL
    src/*.h
    src/*.cpp
    src/*.cc
    src/*.c
)

add_library(InworldNDK ${FilesALL})

target_compile_definitions(InworldNDK PRIVATE
    NDK_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NDK_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NDK_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    NDK_VERSION_STRING="${PROJECT_VERSION}"
)

if(ANDROID)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
    target_compile_definitions(InworldNDK PRIVATE ANDROID=1)
endif()

set(GrpcDir ${CMAKE_CURRENT_BINARY_DIR}/test/grpc)

target_include_directories(InworldNDK PUBLIC
    ${ThirdPartyDir}/include
    ${GrpcDir}/include
    ${GrpcDir}/third_party/protobuf/src
    src
    src/proto
    src/ThirdParty
)

target_compile_features(InworldNDK PUBLIC cxx_std_20)

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
GroupSourcesByFolder(InworldNDK)

if (INWORLD_AUDIO_DUMP)
    target_compile_definitions(InworldNDK PRIVATE INWORLD_AUDIO_DUMP=1)
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
    set(ENABLE_BITCODE ON)
    
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
set(gRPC_RE2_PROVIDER module)
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_SHARED_LIBS OFF)
set(protobuf_INSTALL OFF)
set(utf8_range_ENABLE_INSTALL OFF)

set(gRPC_ABSL_PROVIDER module)
set(gRPC_CARES_PROVIDER module)
set(gRPC_PROTOBUF_PROVIDER module)

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

#set(BUILD_SHARED_LIBS ON)
set(gRPC_INSTALL OFF)

add_subdirectory(test/grpc)

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

# Unity Setup
option(UNITY FALSE)
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
    set(PackageLibDir  ${PackageDir}/lib/Android/${CMAKE_ANDROID_ARCH_ABI})
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

set(LibFiles 
    #${CMAKE_CURRENT_BINARY_DIR}${ConfigDirName}/${LibPref}InworldNDK.${LibExt}
    #${AbslDir}/base${ConfigDirName}/${LibPref}absl_base.${LibExt}
    #${AbslDir}/base${ConfigDirName}/${LibPref}absl_malloc_internal.${LibExt}
    #${AbslDir}/base${ConfigDirName}/${LibPref}absl_raw_logging_internal.${LibExt}
    #${AbslDir}/base${ConfigDirName}/${LibPref}absl_spinlock_wait.${LibExt}
    #${AbslDir}/base${ConfigDirName}/${LibPref}absl_throw_delegate.${LibExt}
    #${AbslDir}/types${ConfigDirName}/${LibPref}absl_bad_optional_access.${LibExt}
    #${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cord.${LibExt}
    #${AbslDir}/strings${ConfigDirName}/${LibPref}absl_str_format_internal.${LibExt}
    #${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings.${LibExt}
    #${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings_internal.${LibExt}
    #${AbslDir}/debugging${ConfigDirName}/${LibPref}absl_symbolize.${LibExt}
    #${AbslDir}/debugging${ConfigDirName}/${LibPref}absl_stacktrace.${LibExt}
    #${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_graphcycles_internal.${LibExt}
    #${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_synchronization.${LibExt}
    #${AbslDir}/numeric${ConfigDirName}/${LibPref}absl_int128.${LibExt}
    #${AbslDir}/status${ConfigDirName}/${LibPref}absl_status.${LibExt}
    #${AbslDir}/status${ConfigDirName}/${LibPref}absl_statusor.${LibExt}
    #${AbslDir}/time${ConfigDirName}/${LibPref}absl_time.${LibExt}
    #${AbslDir}/time${ConfigDirName}/${LibPref}absl_time_zone.${LibExt}
    #${GrpcDir}${ConfigDirName}/${LibPref}address_sorting.${LibExt}
    #${GrpcDir}${ConfigDirName}/${LibPref}gpr.${LibExt}
    #${GrpcDir}${ConfigDirName}/${LibPref}grpc.${LibExt}
    #${GrpcDir}${ConfigDirName}/${LibPref}grpc++.${LibExt}
    #${GrpcDir}/third_party/cares/cares/lib${ConfigDirNameCares}/${LibPref}cares.${LibExt}
    #${GrpcDir}/third_party/protobuf${ConfigDirName}/${LibProtobufName}.${LibExt}
    #${GrpcDir}/third_party/re2${ConfigDirName}/${LibPref}re2.${LibExt}
    #${GrpcDir}${ConfigDirName}/${LibPref}upb.${LibExt}

    ${AbslDir}/status${ConfigDirName}/${LibPref}absl_statusor.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags.${LibExt}
    ${AbslDir}/status${ConfigDirName}/${LibPref}absl_status.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_reflection.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cord.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cordz_info.${LibExt}
    ${AbslDir}/container${ConfigDirName}/${LibPref}absl_raw_hash_set.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_config.${LibExt}
    ${AbslDir}/container${ConfigDirName}/${LibPref}absl_hashtablez_sampler.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cord_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cordz_handle.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_program_name.${LibExt}
    ${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_synchronization.${LibExt}
    ${AbslDir}/crc${ConfigDirName}/${LibPref}absl_crc_cord_state.${LibExt}
    ${AbslDir}/crc${ConfigDirName}/${LibPref}absl_crc32c.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_marshalling.${LibExt}
    ${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_kernel_timeout_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_str_format_internal.${LibExt}
    ${AbslDir}/time${ConfigDirName}/${LibPref}absl_time.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_seed_sequences.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_pool_urbg.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_private_handle_accessor.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_distributions.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_seed_material.${LibExt}
    ${AbslDir}${ConfigDirName}ging${ConfigDirName}/${LibPref}absl_symbolize.${LibExt}
    ${AbslDir}/hash${ConfigDirName}/${LibPref}absl_hash.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_commandlineflag.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings.${LibExt}
    ${AbslDir}/time${ConfigDirName}/${LibPref}absl_time_zone.${LibExt}
    ${AbslDir}/crc${ConfigDirName}/${LibPref}absl_crc_internal.${LibExt}
    ${AbslDir}/synchronization${ConfigDirName}/${LibPref}absl_graphcycles_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_strings_internal.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_malloc_internal.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_string_view.${LibExt}
    ${AbslDir}/hash${ConfigDirName}/${LibPref}absl_low_level_hash.${LibExt}
    ${AbslDir}/crc${ConfigDirName}/${LibPref}absl_crc_cpu_detect.${LibExt}
    ${AbslDir}/hash${ConfigDirName}/${LibPref}absl_city.${LibExt}
    ${AbslDir}${ConfigDirName}ging${ConfigDirName}/${LibPref}absl_demangle_internal.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_base.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_randen.${LibExt}
    ${AbslDir}${ConfigDirName}ging${ConfigDirName}/${LibPref}absl_stacktrace.${LibExt}
    ${AbslDir}/types${ConfigDirName}/${LibPref}absl_bad_optional_access.${LibExt}
    ${AbslDir}/strings${ConfigDirName}/${LibPref}absl_cordz_functions.${LibExt}
    ${AbslDir}/types${ConfigDirName}/${LibPref}absl_bad_variant_access.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_throw_delegate.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_randen_hwaes.${LibExt}
    ${AbslDir}${ConfigDirName}ging${ConfigDirName}/${LibPref}absl_debugging_internal.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_randen_slow.${LibExt}
    ${AbslDir}/numeric${ConfigDirName}/${LibPref}absl_int128.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_raw_logging_internal.${LibExt}
    ${AbslDir}/time${ConfigDirName}/${LibPref}absl_civil_time.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_randen_hwaes_impl.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_seed_gen_exception.${LibExt}
    ${AbslDir}/profiling${ConfigDirName}/${LibPref}absl_exponential_biased.${LibExt}
    ${AbslDir}/flags${ConfigDirName}/${LibPref}absl_flags_commandlineflag_internal.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_strerror.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_log_severity.${LibExt}
    ${AbslDir}/random${ConfigDirName}/${LibPref}absl_random_internal_platform.${LibExt}
    ${AbslDir}/base${ConfigDirName}/${LibPref}absl_spinlock_wait.${LibExt}
    ${GrpcDir}/third_party/cares/cares/lib${ConfigDirNameCares}/${LibPref}cares.${LibExt}
    #${GrpcDir}/third_party/protobuf${ConfigDirName}/${LibProtobufName}.${LibExt}
    ${GrpcDir}/third_party/re2${ConfigDirName}/${LibPref}re2.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}upb_json_lib.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}upb_textformat_lib.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}upb_collections_lib.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}upb.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}address_sorting.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}utf8_range_lib.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}grpc.${LibExt}
    ${GrpcDir}${ConfigDirName}/${LibPref}gpr.${LibExt}

    ${OPENSSL_SSL_LIBRARY}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${ZLIB_LIBRARY_RELEASE}
)

if (ANDROID AND INWORLD_LOG_SPD)
    list(APPEND LibFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/build/ThirdParty/spdlog/${LibPref}${LibSpglogName}.${LibExt})
endif()

# webrtc aec setup
if (AEC)
    if (WIN32 OR MAC)
        target_compile_definitions(InworldNDK PUBLIC INWORLD_AEC)
        target_include_directories(InworldNDK PUBLIC inworld-aec/src)
        add_custom_command(TARGET InworldNDK POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/src/AECInterop.h  ${PackageDir}/include)
    else()
        message(FATAL_ERROR "AEC: not supported")
    endif()
    
    if (WIN32)
        target_link_libraries(InworldNDK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.lib)

        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.lib
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.dll
        )
    elseif (MAC)
        target_link_libraries(InworldNDK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Mac/libwebrtc_aec_plugin.dylib)

        list(APPEND LibFiles 
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Mac/libwebrtc_aec_plugin.dylib
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
CopyIncludeDir(${GrpcDir}/include)
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
            ${CMAKE_CURRENT_SOURCE_DIR}/inworld-aec/prebuilt/Win64/webrtc_aec_plugin.dll  ${PackageDir}${ConfigDirName})
    endif()
endif()
