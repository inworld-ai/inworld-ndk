cmake_minimum_required (VERSION 3.22)

project(inworld-ndk-vad VERSION 1.0.0)

if(WIN32 OR MAC)
    message("Build VAD")
else()
    message(FATAL_ERROR "VAD: unsupported platform")
endif()

add_library(inworld-ndk-vad SHARED src/InworldVAD.h src/InworldVAD.cpp)
set_target_properties(inworld-ndk-vad PROPERTIES
            LIBRARY_OUTPUT_NAME "inworld-ndk-vad.dll"
            IMPORT_SUFFIX ".dll.lib"
        )

add_subdirectory(third-party/onnxruntime/cmake)
if (MAC)
    target_link_libraries(inworld-ndk-vad PRIVATE nsync::nsync_cpp)
endif()
target_link_libraries(inworld-ndk-vad PRIVATE 
    absl_bad_optional_access
    absl_bad_variant_access
    absl_base
    absl_city
    absl_civil_time
    absl_cord
    absl_cord_internal
    absl_cordz_functions
    absl_cordz_handle
    absl_cordz_info
    absl_crc32c
    absl_crc_cord_state
    absl_crc_cpu_detect
    absl_crc_internal
    absl_debugging_internal
    absl_demangle_internal
    absl_exponential_biased
    absl_flags_commandlineflag
    absl_flags_commandlineflag_internal
    absl_flags_config
    absl_flags_internal
    absl_flags_marshalling
    absl_flags_private_handle_accessor
    absl_flags_program_name
    absl_flags_reflection
    absl_graphcycles_internal
    absl_hash
    absl_hashtablez_sampler
    absl_int128
    absl_kernel_timeout_internal
    absl_log_severity
    absl_low_level_hash
    absl_malloc_internal
    absl_raw_hash_set
    absl_raw_logging_internal
    absl_spinlock_wait
    absl_stacktrace
    absl_str_format_internal
    absl_string_view
    absl_strings
    absl_strings_internal
    absl_symbolize
    absl_synchronization
    absl_throw_delegate
    absl_time
    absl_time_zone
    clog
    cpuinfo
    flatbuffers
#    libprotobuf-lite
    onnx
    onnx_proto
    onnxruntime_common
    onnxruntime_flatbuffers
    onnxruntime_framework
#    onnxruntime_generate_def
    onnxruntime_graph
    onnxruntime_mlas
    onnxruntime_optimizer
    onnxruntime_providers
    onnxruntime_session
    onnxruntime_util
    re2
)
target_compile_definitions(onnxruntime_common PRIVATE -D_SILENCE_ALL_CXX23_DEPRECATION_WARNINGS)
target_include_directories(inworld-ndk-vad PRIVATE third-party/onnxruntime/include/onnxruntime/core/session)
target_compile_features(inworld-ndk-vad PRIVATE cxx_std_17)
target_compile_options(inworld-ndk-vad PRIVATE -std=c++17)
if (WIN32)
    add_custom_command(TARGET inworld-ndk-vad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/inworld-ndk-vad.dll ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64)
    add_custom_command(TARGET inworld-ndk-vad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/inworld-ndk-vad.dll.lib ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Win64)
elseif(MAC)
    add_custom_command(TARGET inworld-ndk-vad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/build/$<CONFIGURATION>/libinworld-ndk-vad.dylib ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/Mac)
endif()

#add_executable(inworld-ndk-vad-app src/Main.cpp)
#target_link_libraries(inworld-ndk-vad-app PRIVATE inworld-ndk-vad)

    