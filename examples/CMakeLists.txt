include_directories("${PROJECT_SOURCE_DIR}/libclandmark")

find_package( OpenCV REQUIRED core imgproc objdetect highgui )
include_directories(${OpenCV_INCLUDE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/3rd_party/rapidxml-1.13)
include_directories(${PROJECT_SOURCE_DIR}/3rd_party/CImg-1.5.6)

## Several examples using flandmark
set(EXAMPLES static_input video_input)
foreach(var ${EXAMPLES})
    add_executable(${var} ${var}.cpp)
    target_link_libraries(${var} flandmark ${OpenCV_LIBS})
endforeach()

install(TARGETS ${EXAMPLES} EXPORT CLandmarkTargets DESTINATION share/clandmark/examples COMPONENT Examples)

