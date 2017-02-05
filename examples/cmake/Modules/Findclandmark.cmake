# - Try to find the clandmark landmark detector library
#
# =============================================================================
# Once done this will define:
#
#  CLANDMARK_FOUND          TRUE if found; FALSE otherwise
#  CLANDMARK_INCLUDE_DIRS   where to find flandmark_detector.h
#  CLANDMARK_LIBRARIES      the libraries to link against
#
# =============================================================================
# Variables used by this module:
#
#  CLANDMARK_PREFER_STATIC  If TRUE and available, link against the static
#                           flandmark library. Otherwise select the shared
#                           version
#
# =============================================================================
# To use this from another project:
#
# create a directory named cmake/Modules under the project root, copy this file
# (FindCLANDMARK.cmake) there, and in the top-level CMakeLists.txt include:
#   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
#                         "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#
# =============================================================================

find_path(CLANDMARK_INCLUDE_DIR CLandmark.h)
find_path(FLANDMARK_INCLUDE_DIR Flandmark.h)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".dll" ".dylib")
find_library(CLANDMARK_LIBRARY_SHARED NAMES clandmark)
find_library(FLANDMARK_LIBRARY_SHARED NAMES flandmark)

#
if (UNIX)
	set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
endif(UNIX)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib")
find_library(CLANDMARK_LIBRARY_STATIC NAMES clandmark)
find_library(FLANDMARK_LIBRARY_STATIC NAMES flandmark)


# set(CLANDMARK_LIBRARY ${CLANDMARK_LIBRARY_STATIC})
set(CLANDMARK_LIBRARY ${CLANDMARK_LIBRARY_SHARED})
# set(CLANDMARK_LIBRARY ${CLANDMARK_LIBRARY_STATIC} ${CLANDMARK_LIBRARY_SHARED})
set(CLANDMARK_LIBRARIES ${CLANDMARK_LIBRARY})
set(CLANDMARK_INCLUDE_DIRS ${CLANDMARK_INCLUDE_DIR})

set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_SHARED})
# set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_STATIC} ${FLANDMARK_LIBRARY_SHARED})
set(FLANDMARK_LIBRARIES ${FLANDMARK_LIBRARY})
set(FLANDMARK_INCLUDE_DIRS ${FLANDMARK_INCLUDE_DIR})

# Temporary DEBUG message
message(STATUS "CLANDMARK: ${CLANDMARK_INCLUDE_DIR}, ${CLANDMARK_LIBRARIES}")
message(STATUS "FLANDMARK: ${FLANDMARK_INCLUDE_DIR}, ${FLANDMARK_LIBRARIES}")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CLANDMARK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  CLANDMARK
  DEFAULT_MSG
  CLANDMARK_LIBRARY CLANDMARK_INCLUDE_DIR
)

find_package_handle_standard_args(
  FLANDMARK
  DEFAULT_MSG
  FLANDMARK_LIBRARY FLANDMARK_INCLUDE_DIR
)

mark_as_advanced(CLANDMARK_INCLUDE_DIR CLANDMARK_LIBRARY)
mark_as_advanced(FLANDMARK_INCLUDE_DIR FLANDMARK_LIBRARY)
