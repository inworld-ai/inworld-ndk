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
if (NOT APPLE)
    target_compile_definitions(InworldNDK PUBLIC INWORLD_NDK_LOG=1)
    target_compile_definitions(InworldNDK PUBLIC SPDLOG_USE_STD_FORMAT=1)
endif()

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

#set(CMAKE_CXX_STANDARD_LIBRARIES "Crypt32.Lib User32.lib Advapi32.lib")
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
set(OPENSSL_CRYPTO_LIBRARY ${LIB_EAY_LIBRARY_DEBUG})
#set(LIB_EAY_DEBUG ${LIB_EAY_LIBRARY_DEBUG})
#set(LIB_EAY_RELEASE ${LIB_EAY_LIBRARY_DEBUG})
#set(SSL_EAY_DEBUG ${LIB_EAY_LIBRARY_DEBUG})

set(OPENSSL_INCLUDE_DIR ${OpenSSLDir}/${OpensslIncludeSubpath})
set(OPENSSL_ROOT_DIR ${OpenSSLDir})
set(OPENSSL_SSL_LIBRARY ${OpenSSLDir}/${LibsslPlatformSubpath})
#set(SSL_EAY_LIBRARY_DEBUG ${OpenSSLDir}/${LibsslPlatformSubpath})
#set(SSL_EAY_LIBRARY_RELEASE ${SSL_EAY_LIBRARY_DEBUG})
#set(SSL_EAY_RELEASE ${SSL_EAY_LIBRARY_DEBUG})

ChechPathValid(${ZLIB_INCLUDE_DIR})
ChechPathValid(${ZLIB_LIBRARY_RELEASE})
ChechPathValid(${OPENSSL_CRYPTO_LIBRARY})
ChechPathValid(${OPENSSL_ROOT_DIR})
ChechPathValid(${OPENSSL_SSL_LIBRARY})

add_subdirectory(ThirdParty/grpc)

target_link_libraries(InworldNDK PUBLIC grpc)
target_link_libraries(InworldNDK PUBLIC grpc++)

# spdlog


add_subdirectory(ThirdParty/spdlog)
target_link_libraries(InworldNDK PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

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
if (Win32)
    add_custom_command(
        TARGET InworldNDKUnit POST_BUILD
        COMMAND InworldNDKUnit
        VERBATIM
    )
endif()

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
    set(LibExt lib)
    set(LibPref "")
elseif (APPLE)
    set(OutputLibDir ${PROJECT_SOURCE_DIR}/build/lib/Mac)
    set(LibExt a)
    set(LibPref lib)
endif()

add_custom_command(TARGET InworldNDK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OutputLibDir})

set(LibFiles 
    ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${LibPref}InworldNDK.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/types/$<CONFIGURATION>/${LibPref}absl_bad_optional_access.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/${LibPref}absl_base.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/${LibPref}absl_cord.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/synchronization/$<CONFIGURATION>/${LibPref}absl_graphcycles_internal.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/numeric/$<CONFIGURATION>/${LibPref}absl_int128.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/${LibPref}absl_malloc_internal.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/${LibPref}absl_raw_logging_internal.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/${LibPref}absl_spinlock_wait.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/debugging/$<CONFIGURATION>/${LibPref}absl_stacktrace.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/status/$<CONFIGURATION>/${LibPref}absl_status.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/status/$<CONFIGURATION>/${LibPref}absl_statusor.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/${LibPref}absl_str_format_internal.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/${LibPref}absl_strings.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/strings/$<CONFIGURATION>/${LibPref}absl_strings_internal.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/debugging/$<CONFIGURATION>/${LibPref}absl_symbolize.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/synchronization/$<CONFIGURATION>/${LibPref}absl_synchronization.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/base/$<CONFIGURATION>/${LibPref}absl_throw_delegate.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/time/$<CONFIGURATION>/${LibPref}absl_time.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/abseil-cpp/absl/time/$<CONFIGURATION>/${LibPref}absl_time_zone.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/${LibPref}address_sorting.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/cares/cares/lib/$<CONFIGURATION>/${LibPref}cares.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/${LibPref}gpr.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/${LibPref}grpc.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/${LibPref}grpc++.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/protobuf/$<CONFIGURATION>/libprotobuf.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/third_party/re2/$<CONFIGURATION>/${LibPref}re2.${LibExt}
    ${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/grpc/$<CONFIGURATION>/${LibPref}upb.${LibExt}
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
