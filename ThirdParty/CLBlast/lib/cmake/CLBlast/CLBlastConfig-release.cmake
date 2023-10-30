#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "clblast" for configuration "Release"
set_property(TARGET clblast APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(clblast PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/clblast.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "C:/vcpkg/packages/opencl_x64-windows/lib/OpenCL.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/clblast.dll"
  )

list(APPEND _cmake_import_check_targets clblast )
list(APPEND _cmake_import_check_files_for_clblast "${_IMPORT_PREFIX}/lib/clblast.lib" "${_IMPORT_PREFIX}/lib/clblast.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
