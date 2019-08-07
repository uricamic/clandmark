# python_interface/src/CMakeLists.txt

find_package(python)

# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/py_flandmark.pyx PROPERTIES CYTHON_IS_CXX TRUE)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/py_featurePool.pyx PROPERTIES CYTHON_IS_CXX TRUE)

# Multi-file cython modules do not appear to be working at the moment.
cython_add_module(py_featurePool py_featurePool.pyx)

# message(STATUS "CLANDMARK_LIBRARIES=${CLANDMARK_LIBRARIES}")
# message(STATUS "FLANDMARK_LIBRARIES=${FLANDMARK_LIBRARIES}")
message(STATUS "PYTHON_LIBRARIES=${PYTHON_LIBRARIES}")
message(STATUS "Python_LIBRARIES=${Python_LIBRARIES}")

#target_link_libraries(py_featurePool ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES})
target_link_libraries(py_featurePool ${FLANDMARK_LIBRARY_STATIC} ${CLANDMARK_LIBRARY_STATIC})	# for MSVC now, change _LIBRARIES to contain .lib files also, to solve this

## TRY THIS
if(APPLE)
	set_target_properties(py_featurePool PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else(APPLE)
	if (UNIX)
		message(STATUS "Linking target py_featurePool against libpython")
		target_link_libraries(py_featurePool ${PYTHON_LIBRARIES})
		# target_link_libraries(py_featurePool ${Python_LIBRARIES})
	endif(UNIX)
	if (MSVC)
		message(STATUS "Not linking target py_featurePool against libpython")
		set_target_properties(py_featurePool PROPERTIES LINK_FLAGS "/LTCG")
		target_link_libraries(py_featurePool ${PYTHON_LIBRARIES})
		# target_link_libraries(py_featurePool ${Python_LIBRARIES})
	endif(MSVC)
endif(APPLE)
#########

cython_add_module(py_flandmark py_flandmark.pyx)
# cython_add_module2( py_flandmark py_flandmark.pyx TRUE )
# target_link_libraries(py_flandmark ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES})
target_link_libraries(py_flandmark ${FLANDMARK_LIBRARY_STATIC} ${CLANDMARK_LIBRARY_STATIC}) # for MSVC now, change _LIBRARIES to contain .lib files also, to solve this s

## TRY THIS
if(APPLE)
	set_target_properties(py_flandmark PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else(APPLE)
	if (UNIX)
		message(STATUS "Linking target py_flandmark against libpython")
		target_link_libraries(py_flandmark ${PYTHON_LIBRARIES})
	endif(UNIX)
	if (MSVC)
		message(STATUS "Not linking target py_flandmark against libpython")
		# set_target_properties(py_flandmark PROPERTIES LINK_FLAGS "/LTCG")
		target_link_libraries(py_flandmark ${PYTHON_LIBRARIES})
		# target_link_libraries(py_flandmark ${Python_LIBRARIES})
	endif(MSVC)
endif(APPLE)
#########

add_custom_target(${PY_NAME_interface} ALL DEPENDS py_flandmark)

#TODO: copy libclandmark and libflandmark

if (UNIX)
	set(copy_py_flandmark "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_flandmark${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
	set(copy_py_featurePool "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_featurePool${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
endif (UNIX)

# if (MSVC)
	# set(copy_py_flandmark "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_flandmark${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
	# set(copy_py_featurePool "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_featurePool${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
	## set(copy_py_flandmark "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_flandmark${CMAKE_STATIC_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
	## set(copy_py_featurePool "${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_featurePool${CMAKE_STATIC_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/python_interface/bin")
# endif(MSVC)

# add_custom_command(
    # TARGET ${PY_NAME_interface}
    # POST_BUILD
    ##COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_flandmark.so ${PROJECT_BINARY_DIR}/python_interface/bin
    ##COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/python_interface/src/py_featurePool.so  ${PROJECT_BINARY_DIR}/python_interface/bin

	# COMMAND ${copy_py_flandmark}
	# COMMAND ${copy_py_featurePool}

##    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libclandmark${CMAKE_SHARED_LIBRARY_SUFFIX}.${clandmark_VERSION_MAJOR} ${PROJECT_BINARY_DIR}/python_interface/bin
##    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/libclandmark/libflandmark${CMAKE_SHARED_LIBRARY_SUFFIX}.${clandmark_VERSION_MAJOR} ${PROJECT_BINARY_DIR}/python_interface/bin
##    COMMAND ${CMAKE_COMMAND} -E copy ${FLANDMARK_LIBRARIES} ${PROJECT_BINARY_DIR}/python_interface/bin
##    COMMAND ${CMAKE_COMMAND} -E copy ${CLANDMARK_LIBRARIES} ${PROJECT_BINARY_DIR}/python_interface/bin
    # COMMENT "Copying python_interface/src/py_flandmark${CMAKE_SHARED_LIBRARY_SUFFIX} and python_interface/src/py_featurePool${CMAKE_SHARED_LIBRARY_SUFFIX}"
	## COMMENT "Copying python_interface/src/py_flandmark${CMAKE_SHARED_STATIC_SUFFIX} and python_interface/src/py_featurePool${CMAKE_STATIC_LIBRARY_SUFFIX}"
# )

if (MSVC)
	install(
		FILES ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_flandmark.pyd
		DESTINATION share/clandmark/python
		COMPONENT Python
	)
	install(
		FILES ${PROJECT_BINARY_DIR}/python_interface/src/Release/py_featurePool.pyd
		DESTINATION share/clandmark/python
		COMPONENT Python
	)
endif (MSVC)