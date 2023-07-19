# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("inworldexample")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        inworldexample

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        inworldexample

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

# link inworld

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

target_link_libraries(inworldexample
        android
        log)

target_compile_definitions(inworldexample PUBLIC INWORLD_LOG=1)
target_compile_definitions(inworldexample PUBLIC ANDROID=1)