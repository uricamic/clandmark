# python_interface/src/CMakeLists.txt

# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/py_flandmark.pyx PROPERTIES CYTHON_IS_CXX TRUE )
set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/py_featurePool.pyx PROPERTIES CYTHON_IS_CXX TRUE )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wlong-long,-Wl,-rpath,'$ORIGIN/' " )

# Multi-file cython modules do not appear to be working at the moment.
cython_add_module( py_featurePool py_featurePool.pyx )
target_link_libraries( py_featurePool ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES} )

cython_add_module( py_flandmark py_flandmark.pyx )
target_link_libraries( py_flandmark ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES} )

add_custom_target(${PY_NAME_interface} ALL DEPENDS py_flandmark)

#TODO: copy libclandmark and libflandmark

add_custom_command(
    TARGET ${PY_NAME_interface}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_flandmark.so ${PROJECT_BINARY_DIR}/python_interface/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_featurePool.so  ${PROJECT_BINARY_DIR}/python_interface/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libclandmark${CMAKE_SHARED_LIBRARY_SUFFIX}.${clandmark_VERSION_MAJOR} ${PROJECT_BINARY_DIR}/python_interface/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libflandmark${CMAKE_SHARED_LIBRARY_SUFFIX}.${clandmark_VERSION_MAJOR} ${PROJECT_BINARY_DIR}/python_interface/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${FLANDMARK_LIBRARIES} ${PROJECT_BINARY_DIR}/python_interface/bin
#    COMMAND ${CMAKE_COMMAND} -E copy ${CLANDMARK_LIBRARIES} ${PROJECT_BINARY_DIR}/python_interface/bin
    COMMENT "Copying python_interface/src/py_flandmark${CMAKE_SHARED_LIBRARY_SUFFIX}"
)
