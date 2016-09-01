set(LIB_NAME BoostAsioClient)

set(INCLUDE
    "Include/BoostAsioClient/SyncClient.h"
    "Include/BoostAsioClient/AsyncClient.h"
    "Include/BoostAsioClient/StacklessCoroutineClient.h"
    "Include/BoostAsioClient/StackfulCoroutineClient.h")
set(SOURCE
    "Src/SyncClient.cpp"
    "Src/AsyncClient.cpp"
    "Src/StacklessCoroutineClient.cpp"
    "Src/StackfulCoroutineClient.cpp")

source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

include_directories(./Include)
include_directories(${PROJECT_SOURCE_DIR}/Core/Include)
#Boost.Asio support
include_directories(${Boost_INCLUDE_DIRS})

add_library(${LIB_NAME} STATIC ${INCLUDE} ${SOURCE})

target_link_libraries(${LIB_NAME} Core)
#Boost.Asio support
target_link_libraries(${LIB_NAME} ${Boost_LIBRARIES})

#disable Boost autolinking
add_definitions("-DBOOST_ALL_NO_LIB")
#disable warning for WIN32_WINNT in boost
add_definitions("-D_WIN32_WINNT=0x0501")
