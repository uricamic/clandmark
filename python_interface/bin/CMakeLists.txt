# python_interface/bin/CMakeLists.txt

# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx PROPERTIES CYTHON_IS_CXX TRUE)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN/' " )

cython_add_standalone_executable(
    flandmark-demo MAIN_MODULE flandmark_demo.py
    ${PROJECT_SOURCE_DIR}/python_interface/src/py_flandmark.pyx
    flandmark_demo.py
)

# target_link_libraries(flandmark-demo ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES})
target_link_libraries(flandmark-demo ${FLANDMARK_LIBRARY_STATIC} ${CLANDMARK_LIBRARY_STATIC})
## TRY THIS
if(APPLE)
	set_target_properties(flandmark-demo PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else(APPLE)
	if (UNIX)
		message(STATUS "Linking target flandmark-demo against libpython")
		target_link_libraries(flandmark-demo ${PYTHON_LIBRARIES})
	endif(UNIX)
	if (MSVC)
		message(STATUS "Not linking target flandmark-demo against libpython")
		set_target_properties(flandmark-demo PROPERTIES LINK_FLAGS "/LTCG")
	endif(MSVC)
endif(APPLE)
#########
