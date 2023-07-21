# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

project("inworldexample")

add_library( # Sets the name of the library.
        inworldexample
        SHARED
        native-lib.cpp)

find_library(log-lib log)
target_link_libraries(inworldexample ${log-lib})

# link Inworld
set(InworldNDKPackageDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../build/Package)
set(LibDir ${InworldNDKPackageDir}/lib/Android)

set(LibFiles
    ${LibDir}/libInworldNDK.a
    ${LibDir}/libabsl_base.a
    ${LibDir}/libabsl_malloc_internal.a
    ${LibDir}/libabsl_raw_logging_internal.a
    ${LibDir}/libabsl_spinlock_wait.a
    ${LibDir}/libabsl_throw_delegate.a
    ${LibDir}/libabsl_bad_optional_access.a
    ${LibDir}/libabsl_cord.a
    ${LibDir}/libabsl_str_format_internal.a
    ${LibDir}/libabsl_strings.a
    ${LibDir}/libabsl_strings_internal.a
    ${LibDir}/libabsl_symbolize.a
    ${LibDir}/libabsl_stacktrace.a
    ${LibDir}/libabsl_graphcycles_internal.a
    ${LibDir}/libabsl_synchronization.a
    ${LibDir}/libabsl_int128.a
    ${LibDir}/libabsl_status.a
    ${LibDir}/libabsl_statusor.a
    ${LibDir}/libabsl_time.a
    ${LibDir}/libabsl_time_zone.a
    ${LibDir}/libaddress_sorting.a
    ${LibDir}/libgpr.a
    ${LibDir}/libgrpc.a
    ${LibDir}/libgrpc++.a
    ${LibDir}/libcares.a
    ${LibDir}/libprotobuf.a
    ${LibDir}/libre2.a
    ${LibDir}/libupb.a
    ${LibDir}/libcrypto.a
    ${LibDir}/libssl.a
    ${LibDir}/libz.a
    ${LibDir}/libspdlog.a
)

foreach(LibFile IN LISTS LibFiles)
    target_link_libraries(inworldexample ${LibFile})
endforeach()

target_compile_definitions(inworldexample PUBLIC INWORLDAINDK_API=)

target_include_directories(inworldexample PUBLIC
    ${InworldNDKPackageDir}/include
    ${InworldNDKPackageDir}/src
    ${InworldNDKPackageDir}/src/proto
)

target_link_libraries(inworldexample android log)

target_compile_definitions(inworldexample PUBLIC INWORLD_LOG=1)
target_compile_definitions(inworldexample PUBLIC ANDROID=1)