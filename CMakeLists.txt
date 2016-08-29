cmake_minimum_required(VERSION 3.1)

project(BoostAsioPractice)

option(USE_LOCAL_THIRD_PARTY_LIBRARY 
    "On to search external librarys in ThirdParty folder. OFF to search in operation system." ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Setting Boost dependency
if(USE_LOCAL_THIRD_PARTY_LIBRARY)
    set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/ThirdParty/boost)
    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
else()
    find_package(Boost 1.60 REQUIRED)
    message("Boost:" ${BOOST_ROOT})
    message("Boost:" ${BOOST_INCLUDEDIR})
    message("Boost:" ${BOOST_LIBRARYDIR})
endif()

add_subdirectory(Core)
add_subdirectory(BoostAsioServer)
add_subdirectory(BoostAsioClient)
add_subdirectory(ThirdParty)