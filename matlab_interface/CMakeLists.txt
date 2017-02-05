set(MEX_NAME_flandmark_interface flandmark_interface)
set(MEX_NAME_featuresPool_interface featuresPool_interface)

# create mex subdirectory
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/matlab_interface/mex)

if(WIN32)
	find_program(MEX_CMD mex.bat)
	find_program(MEXEXT_CMD mexext.bat)
else(WIN32)
	find_program(MEX_CMD mex)
	find_program(MEXEXT_CMD mexext)
endif(WIN32)

if(MEX_CMD AND MEXEXT_CMD)

	get_filename_component(MEX_REAL_CMD ${MEX_CMD} ABSOLUTE)
	get_filename_component(MEX_PATH ${MEX_REAL_CMD} PATH)

	get_filename_component(MEXEXT_REAL_CMD ${MEXEXT_CMD} ABSOLUTE)
	get_filename_component(MEXEXT_PATH ${MEXEXT_REAL_CMD} PATH)

	if(MEX_PATH STREQUAL MEXEXT_PATH)

		execute_process(COMMAND ${MEXEXT_REAL_CMD} OUTPUT_VARIABLE MEX_EXTENSION OUTPUT_STRIP_TRAILING_WHITESPACE)

		set(MEX_FILE_flandmark_interface ${CMAKE_CURRENT_BINARY_DIR}/${MEX_NAME_flandmark_interface}.${MEX_EXTENSION})
		set(MEX_FILE_featuresPool_interface ${CMAKE_CURRENT_BINARY_DIR}/${MEX_NAME_featuresPool_interface}.${MEX_EXTENSION})

		if (UNIX)
			set(add_libclandmark "${PROJECT_BINARY_DIR}/libclandmark/libclandmark.a")
			set(add_libflandmark "${PROJECT_BINARY_DIR}/libclandmark/libflandmark.a")
		endif (UNIX)
		
		if (WIN32)
			set(add_libclandmark "${PROJECT_BINARY_DIR}/libclandmark/Release/clandmark.lib")
			set(add_libflandmark "${PROJECT_BINARY_DIR}/libclandmark/Release/flandmark.lib")
		endif (WIN32)
		
		 add_custom_command(
			OUTPUT ${MEX_FILE_flandmark_interface}
			COMMAND ${MEX_REAL_CMD}
			ARGS -v -largeArrayDims #${MEX_DEFINES}
			${CMAKE_CURRENT_SOURCE_DIR}/flandmark_interface_mex.cpp
			-I${PROJECT_BINARY_DIR}	# DOUBLE_PRECISION
			-I${PROJECT_SOURCE_DIR}/libclandmark
			-I${RapidXML_INCLUDE_DIR}
			-I${CImg_INCLUDE_DIR}
			-I${CImg_INCLUDE_DIR}/plugins
			${add_libflandmark}
			${add_libclandmark}
			-output ${MEX_NAME_flandmark_interface}
			DEPENDS flandmark ${CMAKE_CURRENT_SOURCE_DIR}/flandmark_interface_mex.cpp
			COMMENT "Building MEX extension ${MEX_FILE_flandmark_interface}"
		)
		
		add_custom_command(
			OUTPUT ${MEX_FILE_featuresPool_interface}
			COMMAND ${MEX_REAL_CMD}
			ARGS -v -largeArrayDims #${MEX_DEFINES}
			${CMAKE_CURRENT_SOURCE_DIR}/featuresPool_interface_mex.cpp
			${add_libflandmark}
			${add_libclandmark}
			-I${PROJECT_SOURCE_DIR}/libclandmark
			-output ${MEX_NAME_featuresPool_interface}
			DEPENDS flandmark ${CMAKE_CURRENT_SOURCE_DIR}/featuresPool_interface_mex.cpp
			COMMENT "Building MEX extension ${MEX_FILE_featuresPool_interface}"
		)
		
		add_custom_target(${MEX_NAME_flandmark_interface} ALL DEPENDS ${MEX_FILE_flandmark_interface})
		add_custom_target(${MEX_NAME_featuresPool_interface} ALL DEPENDS ${MEX_FILE_featuresPool_interface})

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

		install(FILES ${MEX_FILE_flandmark_interface} DESTINATION share/clandmark/mex COMPONENT Matlab)
		install(FILES ${MEX_FILE_featuresPool_interface} DESTINATION share/clandmark/mex COMPONENT Matlab)

	else(MEX_PATH STREQUAL MEXEXT_PATH)

	    message(WARNING "The 'mex' and 'mexext' programs have been found in different locations. It's likely that one of them is not part of the MATLAB installation. Make sure that the 'bin' directory from the MATLAB installation is in PATH")
	    set(BUILD_MATLAB_BINDINGS OFF)

	endif(MEX_PATH STREQUAL MEXEXT_PATH)

endif(MEX_CMD AND MEXEXT_CMD)
