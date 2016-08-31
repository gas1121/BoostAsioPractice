set(CATCH_SEARCH_PATH "${CATCH_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/ThirdParty/Catch")

find_path(CATCH_INCLUDE_DIR 
    NAMES catch.hpp
    PATHS ${CATCH_SEARCH_PATH})

set(CATCH_INCLUDE_DIRS ${CATCH_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Catch DEFAULT_MSG
    CATCH_INCLUDE_DIR
    CATCH_INCLUDE_DIRS)