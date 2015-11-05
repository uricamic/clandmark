# - Try to find the flandmark landmark detector library
#
# =============================================================================
# Once done this will define:
#
#  FLANDMARK_FOUND          TRUE if found; FALSE otherwise
#  FLANDMARK_INCLUDE_DIRS   where to find flandmark_detector.h
#  FLANDMARK_LIBRARIES      the libraries to link against
#
# =============================================================================
# Variables used by this module:
#
#  FLANDMARK_PREFER_STATIC  If TRUE and available, link against the static
#                           flandmark library. Otherwise select the shared
#                           version
#
# =============================================================================
# To use this from another project:
#
# create a directory named cmake/Modules under the project root, copy this file
# (FindFLANDMARK.cmake) there, and in the top-level CMakeLists.txt include:
#   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
#                         "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#
# =============================================================================

find_path(FLANDMARK_INCLUDE_DIR Flandmark.h)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".dll" ".dylib")
find_library(FLANDMARK_LIBRARY_STATIC NAMES flandmark)

set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib")
find_library(FLANDMARK_LIBRARY_SHARED NAMES flandmark)

#if(FLANDMARK_LIBRARY_STATIC and FLANDMARK_LIBRARY_SHARED)
#	if(FLANDMARK_PREFER_STATIC)
#		set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_STATIC})
#	else()
#		set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_SHARED})
#	endif()
#elseif(FLANDMARK_LIBRARY_STATIC)
#	set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_STATIC})
#elseif(FLANDMARK_LIBRARY_SHARED)
#	set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_SHARED})
#else()
#	set(FLANDMARK_LIBRARY "FLANDMARK_LIBRARY-NOTFOUND")
#endif()

set(FLANDMARK_LIBRARY ${FLANDMARK_LIBRARY_STATIC})
set(FLANDMARK_LIBRARIES ${FLANDMARK_LIBRARY})
set(FLANDMARK_INCLUDE_DIRS ${FLANDMARK_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FLANDMARK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
	FLANDMARK
	DEFAULT_MSG
	FLANDMARK_LIBRARY FLANDMARK_INCLUDE_DIR
)

mark_as_advanced(FLANDMARK_INCLUDE_DIR FLANDMARK_LIBRARY)
