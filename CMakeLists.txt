cmake_minimum_required(VERSION 3.1)

project(BoostAsioPractice)

option(USE_LOCAL_THIRD_PARTY_LIBRARY 
    "On to search external librarys in ThirdParty folder. OFF to search in operation system." ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Setting Boost dependency
if(USE_LOCAL_THIRD_PARTY_LIBRARY)
else()
    find_package(Boost 1.60 REQUIRED)
endif()

add_subdirectory(Core)
add_subdirectory(BoostAsioServer)
add_subdirectory(BoostAsioClient)
add_subdirectory(ThirdParty)