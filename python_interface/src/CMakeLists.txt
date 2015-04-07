#include_directories(${PROJECT_SOURCE_DIR}/python_interface/include)
# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/py_flandmark.pyx
  PROPERTIES CYTHON_IS_CXX TRUE )

set(CLANDMARK_LIBRARIES
    -L${PROJECT_BINARY_DIR}/libclandmark
  	-lflandmark
  	-lclandmark)

set(THREAD_LIBRARIES  
  	-lpthread)

# Multi-file cython modules do not appear to be working at the moment.
cython_add_module( py_flandmark py_flandmark.pyx )

target_link_libraries(py_flandmark ${CLANDMARK_LIBRARIES})

add_custom_target(${PY_NAME_interface} ALL DEPENDS py_flandmark)

add_custom_command(
	TARGET ${PY_NAME_interface} 
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_flandmark.so
	${PROJECT_BINARY_DIR}/python_interface/bin
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libclandmark${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libflandmark${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin
	COMMENT "Copying python_interface/src/py_flandmark${CMAKE_SHARED_LIBRARY_SUFFIX}"
)