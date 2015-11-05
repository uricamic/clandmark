# python_interface/bin/CMakeLists.txt

# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties( ${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx PROPERTIES CYTHON_IS_CXX TRUE )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/' " )

cython_add_standalone_executable(
    flandmark-demo MAIN_MODULE flandmark_demo.py
    ${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx
    flandmark_demo.py
)

target_link_libraries(flandmark-demo ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES})
