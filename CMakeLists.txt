cmake_minimum_required(VERSION 3.1)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

project(BoostAsioPractice)

option(USE_LOCAL_THIRD_PARTY_LIBRARY 
    "Set on to search external librarys in ThirdParty folder, OFF to search in operation system." ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
add_subdirectory(BoostAsioClient)
add_subdirectory(ThirdParty)

install(FILES ${BOOST_Debug_DLLS}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug
    CONFIGURATIONS Debug 
    COMPONENT Boost)
install(FILES ${BOOST_Release_DLLS}
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Release
    CONFIGURATIONS Release 
    COMPONENT Boost)