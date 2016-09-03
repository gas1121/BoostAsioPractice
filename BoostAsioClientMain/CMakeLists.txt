set(EXE_NAME BoostAsioClientMain)

set(SOURCE
    "Src/main.cpp")

source_group("Source" FILES ${SOURCE})

include_directories(${PROJECT_SOURCE_DIR}/Core/Include)
include_directories(${PROJECT_SOURCE_DIR}/BoostAsioClient/Include)
#Boost.Asio support
include_directories(${Boost_INCLUDE_DIRS})

add_executable(${EXE_NAME} ${SOURCE})

target_link_libraries(${EXE_NAME} BoostAsioClient)
#Boost.Asio support
target_link_libraries(${EXE_NAME} ${Boost_LIBRARIES})

#disable Boost autolinking
add_definitions("-DBOOST_ALL_NO_LIB")
#disable warning for WIN32_WINNT in boost
add_definitions("-D_WIN32_WINNT=0x0501")
