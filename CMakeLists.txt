cmake_minimum_required(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

project(BoostAsioPractice)

option(USE_LOCAL_THIRD_PARTY_LIBRARY 
    "Set on to search external librarys in ThirdParty folder, OFF to search in operation system." ON)
option(BUILD_TESTS "Build tests." ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "DEBUG" cache string)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
endif()

# Setting Boost dependency
if(USE_LOCAL_THIRD_PARTY_LIBRARY)
    set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/ThirdParty/boost)
    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
endif()
#Boost.Asio support
find_package(Boost REQUIRED COMPONENTS system date_time regex coroutine chrono thread)

include(Utility)
FILTER_LIST("${Boost_LIBRARIES}" BOOST_Debug_DLLS debug optimized)
FILTER_LIST("${Boost_LIBRARIES}" BOOST_Release_DLLS optimized debug)

add_subdirectory(Core)
add_subdirectory(BoostAsioServer)
add_subdirectory(BoostAsioServerMain)
add_subdirectory(BoostAsioClient)
add_subdirectory(BoostAsioClientMain)
add_subdirectory(ThirdParty)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()

install(FILES ${BOOST_Debug_DLLS}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug
    CONFIGURATIONS Debug 
    COMPONENT Boost)
install(FILES ${BOOST_Release_DLLS}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Release
    CONFIGURATIONS Release 
    COMPONENT Boost)