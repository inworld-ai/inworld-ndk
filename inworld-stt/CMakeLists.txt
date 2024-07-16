cmake_minimum_required (VERSION 3.22)

project(inworld-ndk-stt VERSION 1.0.0)

if(WIN32)
    message("Build STT")
else()
    message(FATAL_ERROR "STT: unsupported platform")
endif()

add_library(inworld-ndk-stt SHARED src/InworldSTT.h src/InworldSTT.cpp)
if (WIN32)
    set_target_properties(inworld-ndk-stt PROPERTIES
        LIBRARY_OUTPUT_NAME "inworld-ndk-stt.dll"
        IMPORT_SUFFIX ".dll.lib"
    )
endif()

option(WITH_MKL "Compile with Intel MKL backend" OFF)
option(WITH_DNNL "Compile with DNNL backend" OFF)
option(WITH_ACCELERATE "Compile with Accelerate backend" OFF)
option(WITH_OPENBLAS "Compile with OpenBLAS backend" ON)
option(WITH_RUY "Compile with Ruy backend" OFF)
option(WITH_CUDA "Compile with CUDA backend" OFF)
option(WITH_CUDNN "Compile with cuDNN backend" OFF)
option(CUDA_DYNAMIC_LOADING "Dynamically load CUDA libraries at runtime" OFF)
option(ENABLE_CPU_DISPATCH "Compile CPU kernels for multiple ISA and dispatch at runtime" ON)
option(ENABLE_PROFILING "Compile with profiling support" OFF)
option(BUILD_CLI "Compile the clients" ON)
option(BUILD_TESTS "Compile the tests" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(WITH_TENSOR_PARALLEL "Compile with NCCL and MPI backend" OFF)
set(OPENMP_RUNTIME "COMP" CACHE STRING "OpenMP runtime (INTEL, COMP, NONE)")
set(OPENBLAS_INCLUDE_DIR "C:/Projects/inworld/OpenBLAS/include")
set(OPENBLAS_LIBRARY "C:/Projects/inworld/OpenBLAS/lib/libopenblas.lib")
add_subdirectory(third-party/ctranslate2)

target_link_libraries(inworld-ndk-stt PRIVATE ctranslate2)
target_include_directories(inworld-ndk-stt PRIVATE third-party/ctranslate2/include)
target_compile_features(inworld-ndk-stt PRIVATE cxx_std_17)
if (WIN32)
    add_custom_command(TARGET inworld-ndk-stt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/inworld-ndk-stt.dll 
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64/inworld-ndk-stt.dll)
    add_custom_command(TARGET inworld-ndk-stt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/inworld-ndk-stt.dll.lib 
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64/inworld-ndk-stt.dll.lib)
    add_custom_command(TARGET inworld-ndk-stt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/third-party/ctranslate2/$<CONFIGURATION>/ctranslate2.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64/ctranslate2.dll)
    add_custom_command(TARGET inworld-ndk-stt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/third-party/ctranslate2/$<CONFIGURATION>/ctranslate2.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64/ctranslate2.lib)
elseif(MAC)
    add_custom_command(TARGET inworld-ndk-stt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/libinworld-ndk-stt.dylib 
        ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Mac/libinworld-ndk-stt.dylib)
endif()

add_executable(inworld-ndk-stt-app src/Main.cpp)
target_link_libraries(inworld-ndk-stt-app PRIVATE inworld-ndk-stt)
target_link_libraries(inworld-ndk-stt-app PRIVATE ctranslate2)
    