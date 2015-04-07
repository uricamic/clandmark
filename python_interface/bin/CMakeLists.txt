# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties( ${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx PROPERTIES CYTHON_IS_CXX TRUE )

set(CLANDMARK_LIBRARIES
    -L${PROJECT_BINARY_DIR}/libclandmark
  	-lflandmark
  	-lclandmark)

cython_add_standalone_executable( flandmark-demo MAIN_MODULE flandmark_demo.py
  	${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx 
  	flandmark_demo.py  	
  )

target_link_libraries(flandmark-demo ${CLANDMARK_LIBRARIES})