set(PY_NAME_interface python_interface)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlong-long")

# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include( UseCython )

# create mex subdirectory

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories( ${PROJECT_SOURCE_DIR}/libclandmark )
include_directories( ${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13 )
include_directories( ${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlong-long") #-Weffc++")

# With CMake, a clean separation can be made between the source tree and the
# build tree.  When all source is compiled, as with pure C/C++, the source is
# no-longer needed in the build tree.  However, with pure *.py source, the
# source is processed directly.  To handle this, we reproduce the availability
# of the source files in the build tree.

add_custom_target( ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P
  ${CMAKE_MODULE_PATH}/ReplicatePythonSourceTree.cmake
  ${CMAKE_CURRENT_BINARY_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

#form example
# Process the CMakeLists.txt in the 'src' and 'bin' directory.
add_subdirectory( src )
add_subdirectory( bin )
