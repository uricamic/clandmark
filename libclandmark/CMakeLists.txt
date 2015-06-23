# prefer preinstalled RapidXML...
find_package(RapidXML)

if(RAPIDXML_FOUND)
    #...but check for known upstream bugs:
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES_BAK "${CMAKE_REQUIRED_INCLUDES}")
    list(APPEND CMAKE_REQUIRED_INCLUDES "${RapidXML_INCLUDE_DIR}")
    check_cxx_source_compiles(
		 "#include <rapidxml_print.hpp>\nint main() { rapidxml::xml_document<> doc; std::string test; rapidxml::print(std::back_inserter(test), doc); return 0; }"
	RAPIDXML_PRINT_HEADER_OK
    )
    if(NOT RAPIDXML_PRINT_HEADER_OK)
       # issue a fatal error here - if we were to use the internal version, we might overwrite the other installation
       message(FATAL_ERROR "RapidXML found but not usable! Please remove preinstalled version.")
    endif()
endif()

if(NOT RAPIDXML_FOUND)
    message(STATUS "RapidXML not found - using internal version.")
    set(RapidXML_INCLUDE_DIR "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13/>" CACHE PATH "Include directory for RapidXML" FORCE)
    file(GLOB RapidXML_HEADERS ${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13/*.hpp)
    install(FILES ${RapidXML_HEADERS} DESTINATION include COMPONENT Devel)
    install(
	FILES
	${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13/manual.html
	${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13/license.txt
	DESTINATION share/doc/clandmark COMPONENT Devel
    )
endif()

# prefer preinstalled CImg:
find_package(CImg)
if(NOT CIMG_FOUND)
    message(STATUS "CImg not found - using internal version.")
    set(CImg_INCLUDE_DIR "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6/>" CACHE PATH "Include directory for CImg" FORCE)
    install(FILES ${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6/CImg.h
	DESTINATION include COMPONENT Devel)
    file(GLOB CImg_DOCS ${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6/*.txt)
    install(FILES ${CImg_DOCS} DESTINATION share/doc/clandmark COMPONENT Devel)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CLandmarkConfig.h.in
    ${PROJECT_BINARY_DIR}/CLandmarkConfig.h
)
set(clandmark_headers
    msvc-compat.h
    base64.h
    CLandmark.h
    CFeaturePool.h
    CFeatures.h
    CAppearanceModel.h
    CDeformationCost.h
    CLoss.h
    CMaxSumSolver.h
    CXMLInOut.h
    CTimer.h
    CTypes.h
    ${PROJECT_BINARY_DIR}/CLandmarkConfig.h
)

set(clandmark_srcs
    ${clandmark_headers}
    CLandmark.cpp
    CFeaturePool.cpp
    CFeatures.cpp
    CAppearanceModel.cpp
    CDeformationCost.cpp
    CLoss.cpp
    CMaxSumSolver.cpp
    CXMLInOut.cpp
)

set(flandmark_headers
    Flandmark.h
    CSparseLBPFeatures.h
    CSparseLBPAppearanceModel.h
##  CExtendedSparseLBPAppearanceModel.h
##  CHOGAppearanceModel.h
    CDisplacementDeformationCost.h
    CNormalizedEuclideanLoss.h
    CTableLoss.h
    CZeroLoss.h
    CTreeMaxSumSolver.h
)

set(flandmark_srcs
    ${flandmark_headers}
    Flandmark.cpp
    CSparseLBPFeatures.cpp
    CSparseLBPAppearanceModel.cpp
##  CExtendedSparseLBPAppearanceModel.cpp
##  CHOGAppearanceModel.cpp
    CDisplacementDeformationCost.cpp
    CNormalizedEuclideanLoss.cpp
    CTableLoss.cpp
    CZeroLoss.cpp
    CTreeMaxSumSolver.cpp
)

# Add OpenMP support
if(USE_OPENMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    else(OPENMP_FOUND)
	set(USE_OPENMP OFF)
    endif(OPENMP_FOUND)
endif(USE_OPENMP)

# CLANDMARK
add_library(clandmark ${clandmark_srcs})
if(NOT ${BUILD_SHARED_LIBS})
    if(NOT MSVC)
	set_target_properties(clandmark PROPERTIES PREFIX "lib")
    endif()
endif()
target_include_directories(clandmark PUBLIC ${CImg_INCLUDE_DIR} ${RapidXML_INCLUDE_DIR})
set_target_properties(clandmark PROPERTIES POSITION_INDEPENDENT_CODE TRUE)	# -fPIC
set_target_properties(clandmark PROPERTIES
    SOVERSION "${clandmark_VERSION_MAJOR}"
    VERSION "${clandmark_VERSION}"
)

# FLANDMARK
add_library(flandmark ${flandmark_srcs})
if(NOT ${BUILD_SHARED_LIBS})
    if(NOT MSVC)
	set_target_properties(flandmark PROPERTIES PREFIX "lib")
    endif()
endif()
target_link_libraries(flandmark clandmark)
set_target_properties(flandmark PROPERTIES POSITION_INDEPENDENT_CODE TRUE)	# -fPIC
set_target_properties(flandmark PROPERTIES
    SOVERSION "${clandmark_VERSION_MAJOR}"
    VERSION "${clandmark_VERSION}"
)

# Models learned distributed with CLandmark
set(flandmark_models
    ${CMAKE_SOURCE_DIR}/data/flandmark_model.xml
    ${CMAKE_SOURCE_DIR}/data/haarcascade_frontalface_alt.xml
)

install(TARGETS clandmark flandmark
    EXPORT CLandmarkTargets
	 LIBRARY DESTINATION lib
	 ARCHIVE DESTINATION lib
	 RUNTIME DESTINATION bin
	 INCLUDES DESTINATION include
)
install(FILES ${clandmark_headers} DESTINATION include COMPONENT Devel)
install(FILES ${flandmark_headers} DESTINATION include COMPONENT Devel)
install(FILES ${flandmark_models} DESTINATION share/clandmark/models)

# deprecated:
##setup Config.cmake
#set(CLANDMARK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/libclandmark" "${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13" "${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6")
#get_property(CLANDMARK_LIBRARIES TARGET clandmark PROPERTY LOCATION)
#configure_file(clandmarkConfig.cmake.in
#    "${PROJECT_BINARY_DIR}/libclandmark/clandmarkConfig.cmake" @ONLY
#)
#
#export(PACKAGE clandmark)
#
#set(FLANDMARK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/libclandmark" "${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13" "${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6")
#get_property(FLANDMARK_LIBRARIES TARGET flandmark PROPERTY LOCATION)
#configure_file(flandmarkConfig.cmake.in
#    "${PROJECT_BINARY_DIR}/libclandmark/flandmarkConfig.cmake" @ONLY
#)
#
#export(PACKAGE flandmark)
