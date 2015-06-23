set(MEX_NAME_flandmark_interface flandmark_interface)
set(MEX_NAME_featuresPool_interface featuresPool_interface)

# create mex subdirectory
if(DOUBLE_PRECISION)
	file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/matlab_interface/mex)
else(DOUBLE_PRECISION)
	file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/matlab_interface/mex_single_precision)
endif(DOUBLE_PRECISION)

if(WIN32)
	find_program(MEX_CMD mex.bat)
	find_program(MEXEXT_CMD mexext.bat)
else(WIN32)
	find_program(MEX_CMD mex)
	find_program(MEXEXT_CMD mexext)
endif(WIN32)

# propagate precision definition
# if(DOUBLE_PRECISION)
# 	add_definitions( -DDOUBLE_PRECISION=1 )
# 	set(MEX_DEFINES "-DDOUBLE_PRECISION=1")
# else(DOUBLE_PRECISION)
# 	add_definitions( -DDOUBLE_PRECISION=0 )
# 	set(MEX_DEFINES "-DDOUBLE_PRECISION=0")
# endif(DOUBLE_PRECISION)

if(MEX_CMD AND MEXEXT_CMD)

	get_filename_component(MEX_REAL_CMD ${MEX_CMD} ABSOLUTE)
	get_filename_component(MEX_PATH ${MEX_REAL_CMD} PATH)

	get_filename_component(MEXEXT_REAL_CMD ${MEXEXT_CMD} ABSOLUTE)
	get_filename_component(MEXEXT_PATH ${MEXEXT_REAL_CMD} PATH)

	if(MEX_PATH STREQUAL MEXEXT_PATH)

	execute_process(COMMAND ${MEXEXT_REAL_CMD} OUTPUT_VARIABLE MEX_EXTENSION OUTPUT_STRIP_TRAILING_WHITESPACE)

	set(MEX_FILE_flandmark_interface ${CMAKE_CURRENT_BINARY_DIR}/${MEX_NAME_flandmark_interface}.${MEX_EXTENSION})
	set(MEX_FILE_featuresPool_interface ${CMAKE_CURRENT_BINARY_DIR}/${MEX_NAME_featuresPool_interface}.${MEX_EXTENSION})

	add_custom_command(
		OUTPUT ${MEX_FILE_flandmark_interface}
		COMMAND ${MEX_REAL_CMD}
		ARGS -v -largeArrayDims #${MEX_DEFINES}
		${CMAKE_CURRENT_SOURCE_DIR}/flandmark_interface_mex.cpp
#		-I${PROJECT_BINARY_DIR}/CLandmarkConfig.h
		-I${PROJECT_BINARY_DIR}	# DOUBLE_PRECISION
		-I${PROJECT_SOURCE_DIR}/libclandmark
		-I${RapidXML_INCLUDE_DIR}
#		-I${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13
		-I${CImg_INCLUDE_DIR}
		-I${CImg_INCLUDE_DIR}/plugins
#		-I${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6
#		-I${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6/plugins
		${PROJECT_BINARY_DIR}/libclandmark/libflandmark.a
		${PROJECT_BINARY_DIR}/libclandmark/libclandmark.a
		-output ${MEX_NAME_flandmark_interface}
		DEPENDS flandmark ${CMAKE_CURRENT_SOURCE_DIR}/flandmark_interface_mex.cpp
		COMMENT "Building MEX extension ${MEX_FILE_flandmark_interface}"
	)

	add_custom_command(
		OUTPUT ${MEX_FILE_featuresPool_interface}
		COMMAND ${MEX_REAL_CMD}
		ARGS -v -largeArrayDims #${MEX_DEFINES}
		${CMAKE_CURRENT_SOURCE_DIR}/featuresPool_interface_mex.cpp
		${PROJECT_BINARY_DIR}/libclandmark/libflandmark.a
		${PROJECT_BINARY_DIR}/libclandmark/libclandmark.a
		-I${PROJECT_SOURCE_DIR}/libclandmark
		-output ${MEX_NAME_featuresPool_interface}
		DEPENDS flandmark ${CMAKE_CURRENT_SOURCE_DIR}/featuresPool_interface_mex.cpp
		COMMENT "Building MEX extension ${MEX_FILE_featuresPool_interface}"
	)


	add_custom_target(${MEX_NAME_flandmark_interface} ALL DEPENDS ${MEX_FILE_flandmark_interface})
	add_custom_target(${MEX_NAME_featuresPool_interface} ALL DEPENDS ${MEX_FILE_featuresPool_interface})

if(DOUBLE_PRECISION)
	add_custom_command(
		TARGET ${MEX_NAME_flandmark_interface}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${MEX_FILE_flandmark_interface} ${PROJECT_SOURCE_DIR}/matlab_interface/mex
		COMMENT "Copying ${MEX_FILE_flandmark_interface}"
	)

	add_custom_command(
		TARGET ${MEX_NAME_featuresPool_interface}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${MEX_FILE_featuresPool_interface} ${PROJECT_SOURCE_DIR}/matlab_interface/mex
		COMMENT "Copying ${MEX_FILE_featuresPool_interface}"
	)
else(DOUBLE_PRECISION)
	add_custom_command(
		TARGET ${MEX_NAME_flandmark_interface}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${MEX_FILE_flandmark_interface} ${PROJECT_SOURCE_DIR}/matlab_interface/mex_single_precision
		COMMENT "Copying ${MEX_FILE_flandmark_interface}"
	)

	add_custom_command(
		TARGET ${MEX_NAME_featuresPool_interface}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${MEX_FILE_featuresPool_interface} ${PROJECT_SOURCE_DIR}/matlab_interface/mex_single_precision
		COMMENT "Copying ${MEX_FILE_featuresPool_interface}"
	)
endif(DOUBLE_PRECISION)

install(FILES ${MEX_FILE_flandmark_interface} DESTINATION share/clandmark/mex COMPONENT Matlab)
install(FILES ${MEX_FILE_featuresPool_interface} DESTINATION share/clandmark/mex COMPONENT Matlab)

	else()

	    message(WARNING "The 'mex' and 'mexext' programs have been found in different locations. It's likely that one of them is not part of the MATLAB installation. Make sure that the 'bin' directory from the MATLAB installation is in PATH")
	    set(BUILD_MATLAB_BINDINGS OFF)

	endif()

endif()
