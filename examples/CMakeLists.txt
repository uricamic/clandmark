# cmake_minimum_required(VERSION 3.0)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include_directories("${PROJECT_SOURCE_DIR}/libclandmark")

# message(STATUS "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

# find_package(clandmark REQUIRED)

message(STATUS "CLANDMARK_INCLUDE_DIRS=${CLANDMARK_INCLUDE_DIRS}")
message(STATUS "CLANDMARK_LIBRARIES=${CLANDMARK_LIBRARIES}")

# find_package(RapidXML)
# if(NOT RAPIDXML_FOUND)
    # message(STATUS "RapidXML not found - using internal version.")
    # set(RapidXML_INCLUDE_DIR "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13/>" CACHE PATH "Include directory for RapidXML" FORCE)
# endif(NOT RAPIDXML_FOUND)

# find_package(CImg)
# if(NOT CIMG_FOUND)
    # message(STATUS "CImg not found - using internal version.")
    # set(CImg_INCLUDE_DIR "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6/>" CACHE PATH "Include directory for CImg" FORCE)
# endif(NOT CIMG_FOUND)

include_directories(
  # ${CImg_INCLUDE_DIR} 
  # ${RapidXML_INCLUDE_DIR}
  ${CLANDMARK_INCLUDE_DIRS}
  ${FLANDMARK_INCLUDE_DIRS}
)

find_package( OpenCV REQUIRED core imgproc objdetect highgui )
include_directories(${OpenCV_INCLUDE_DIR})

# include_directories(${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13)
# include_directories(${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6)

## Several examples using flandmark
set(EXAMPLES static_input video_input)
foreach(var ${EXAMPLES})
    add_executable(${var} ${var}.cpp)
    # target_link_libraries(${var} ${FLANDMARK_LIBRARIES} ${CLANDMARK_LIBRARIES} ${OpenCV_LIBS})
	target_link_libraries(${var} flandmark clandmark ${OpenCV_LIBS})
endforeach()

#install(TARGETS ${EXAMPLES} EXPORT CLandmarkTargets DESTINATION share/clandmark/examples COMPONENT Examples)
install(TARGETS ${EXAMPLES} DESTINATION share/clandmark/examples COMPONENT Examples)
