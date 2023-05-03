cmake_minimum_required (VERSION 3.23)

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

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES Release)

set(ThirdPartyDir ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

file(GLOB_RECURSE FilesALL
    src/*.h
    src/*.cpp
    src/*.cc
    src/*.c
)

add_library(InworldNDK ${FilesALL})

target_include_directories(InworldNDK PUBLIC
    ${ThirdPartyDir}/include
    ${ThirdPartyDir}/grpc/include
    ${ThirdPartyDir}/grpc/third_party/protobuf/src
    src
    src/proto
    src/ThirdParty
)

target_compile_features(InworldNDK PUBLIC cxx_std_20)

target_compile_definitions(InworldNDK PUBLIC INWORLDAINDK_API=)
target_compile_definitions(InworldNDK PUBLIC INWORLD_NDK_LOG=1)
target_compile_definitions(InworldNDK PUBLIC SPDLOG_USE_STD_FORMAT=1)

GroupSourcesByFolder(InworldNDK)

# webrtc aec setup
if (AEC)
    add_subdirectory(ThirdParty/webrtc-aec)
    if (Win32)
        target_link_libraries(InworldNDK PUBLIC ${ThirdPartyDir}/webrtc-aec/webrtc-checkout/src/out/webrtc_aec_plugin.dll.lib)
    endif()
    target_compile_definitions(InworldNDK PUBLIC INWORLD_AEC 1)
endif()

# grpc setup
if(UE_DIR)
    set(ZlibDir ${UE_DIR}/Engine/Source/ThirdParty/zlib/1.2.12)
    set(OpenSSLDir ${UE_DIR}/Engine/Source/ThirdParty/OpenSSL/1.1.1n)
else()
    set(ZlibDir ${ThirdPartyDir}/zlib/1.2.12)
    set(OpenSSLDir ${ThirdPartyDir}/OpenSSL/1.1.1n)
endif()

function(ChechPathValid Path)
    if (NOT EXISTS ${Path})
        message(FATAL_ERROR "Invalid path ${Path}")
    endif()
    message("Valid path ${Path}")
endfunction()

ChechPathValid(${ZlibDir})
ChechPathValid(${OpenSSLDir})

set(CMAKE_CXX_STANDARD_LIBRARIES "Crypt32.Lib User32.lib Advapi32.lib")
set(protobuf_BUILD_TESTS OFF)
set(gRPC_BUILD_TESTS OFF)
set(gRPC_BUILD_CODEGEN OFF)
set(CARES_INSTALL OFF)
set(HAVE_LIBNSL FALSE)

set(gRPC_ZLIB_PROVIDER "package" CACHE STRING "Provider of zlib library")
set(gRPC_SSL_PROVIDER "package" CACHE STRING "Provider of ssl library")

if (Win32)
    set(ZlibPlatformSubpath lib/Win64/Release/zlibstatic.lib)
    set(LibcryptoPlatformSubpath Lib/Win64/VS2015/Release/libcrypto.lib)
    set(LibsslPlatformSubpath Lib/Win64/VS2015/Release/libssl.lib)
    set(OpensslIncludeSubpath include/Win64/VS2015)
elseif (APPLE)
    set(ZlibPlatformSubpath lib/Mac/Release/libz.a)
    set(LibcryptoPlatformSubpath Lib/Mac/libcrypto.a)
    set(LibsslPlatformSubpath Lib/Mac/libssl.a)
    set(OpensslIncludeSubpath include/Mac)
endif()

set(ZLIB_INCLUDE_DIR ${ZlibDir}/include)
set(ZLIB_LIBRARY_DEBUG ${ZlibDir}/${ZlibPlatformSubpath})
set(ZLIB_LIBRARY_RELEASE ${ZLIB_LIBRARY_DEBUG})
set(LIB_EAY_LIBRARY_DEBUG ${OpenSSLDir}/${LibcryptoPlatformSubpath})
set(LIB_EAY_LIBRARY_RELEASE ${LIB_EAY_LIBRARY_DEBUG})
set(LIB_EAY_DEBUG ${LIB_EAY_LIBRARY_DEBUG})
set(LIB_EAY_RELEASE ${LIB_EAY_LIBRARY_DEBUG})
set(SSL_EAY_DEBUG ${LIB_EAY_LIBRARY_DEBUG})
set(OPENSSL_CRYPTO_LIBRARY ${LIB_EAY_LIBRARY_DEBUG})

set(OPENSSL_ROOT_DIR ${OpenSSLDir})
set(SSL_EAY_LIBRARY_DEBUG ${SSL_EAY_DEBUG})
set(SSL_EAY_LIBRARY_RELEASE ${SSL_EAY_DEBUG})
set(SSL_EAY_RELEASE ${SSL_EAY_DEBUG})
set(OPENSSL_INCLUDE_DIR ${SSL_EAY_DEBUG})
set(OPENSSL_SSL_LIBRARY ${SSL_EAY_DEBUG})

ChechPathValid(${ZLIB_INCLUDE_DIR})
ChechPathValid(${ZLIB_LIBRARY_DEBUG})
ChechPathValid(${LIB_EAY_LIBRARY_DEBUG})
ChechPathValid(${SSL_EAY_LIBRARY_DEBUG})

add_subdirectory(ThirdParty/grpc)

target_link_libraries(InworldNDK PUBLIC grpc)
target_link_libraries(InworldNDK PUBLIC grpc++)

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
add_custom_command(
    TARGET InworldNDKUnit POST_BUILD
    COMMAND InworldNDKUnit
    VERBATIM
)

# post build package
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build/include)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build/src)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/build/lib)

if (Win32)
    set(OutputLibDir ${PROJECT_SOURCE_DIR}/build/lib/Win64)
elseif (APPLE)
    set(OutputLibDir ${PROJECT_SOURCE_DIR}/build/lib/Mac)
endif()

add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OutputLibDir})

set(LibFiles 
    ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/InworldNDK.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/types/$<CONFIGURATION>/absl_bad_optional_access.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/absl_base.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/absl_cord.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/synchronization/$<CONFIGURATION>/absl_graphcycles_internal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/numeric/$<CONFIGURATION>/absl_int128.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/absl_malloc_internal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/absl_raw_logging_internal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/absl_spinlock_wait.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/debugging/$<CONFIGURATION>/absl_stacktrace.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/status/$<CONFIGURATION>/absl_status.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/status/$<CONFIGURATION>/absl_statusor.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/absl_str_format_internal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/absl_strings.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/absl_strings_internal.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/debugging/$<CONFIGURATION>/absl_symbolize.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/synchronization/$<CONFIGURATION>/absl_synchronization.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/absl_throw_delegate.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/time/$<CONFIGURATION>/absl_time.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/time/$<CONFIGURATION>/absl_time_zone.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/address_sorting.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/cares/cares/lib/$<CONFIGURATION>/cares.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/gpr.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/grpc.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/grpc++.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/protobuf/$<CONFIGURATION>/libprotobuf.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/re2/$<CONFIGURATION>/re2.lib
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/upb.lib
    ${ZLIB_LIBRARY_RELEASE}
    ${LIB_EAY_LIBRARY_RELEASE}
    ${SSL_EAY_RELEASE}
)

if (AEC)
    if (Win32)
        list(APPEND LibFiles 
            ${ThirdPartyDir}/webrtc-aec/webrtc-checkout/src/out/webrtc_aec_plugin.dll.lib
            ${ThirdPartyDir}/webrtc-aec/webrtc-checkout/src/out/webrtc_aec_plugin.dll
        )
    endif()
endif()

foreach(LibFile IN LISTS LibFiles)
    add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LibFile} ${OutputLibDir})
endforeach()

add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${ThirdPartyDir}/include ${PROJECT_SOURCE_DIR}/build/include)
add_custom_command(TARGET InworldNDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/build/src)
