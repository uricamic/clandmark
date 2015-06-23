# - Try to find Cimg
# Once done this will define
#  CIMG_FOUND - System has CImg
#  CImg_INCLUDE_DIR - The CImg include directories

find_path(CImg_INCLUDE_DIR CImg.h)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CIMG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CImg DEFAULT_MSG CImg_INCLUDE_DIR)

mark_as_advanced(CImg_INCLUDE_DIR)

if(CIMG_FOUND)
    # provide import target:
    add_library(CImg::CImg INTERFACE IMPORTED)
    set_target_properties(CImg::CImg PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CImg_INCLUDE_DIR})
endif()
