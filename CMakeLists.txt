cmake_minimum_required(VERSION 3.1)

project(BoostAsioPractice)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Core)
add_subdirectory(BoostAsioServer)
add_subdirectory(BoostAsioClient)
add_subdirectory(ThirdParty)