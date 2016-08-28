set(EXE_NAME BoostAsioClient)

set(INCLUDE
    "Include/BoostAsioClient/SyncClient.h"
    "Include/BoostAsioClient/AsyncClient.h"
    "Include/BoostAsioClient/StacklessCoroutineClient.h"
    "Include/BoostAsioClient/StackfulCoroutineClient.h")
set(SOURCE
    "Src/main.cpp"
    "Src/SyncClient.cpp"
    "Src/AsyncClient.cpp"
    "Src/StacklessCoroutineClient.cpp"
    "Src/StackfulCoroutineClient.cpp")

source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

include_directories(./Include)
include_directories(${PROJECT_SOURCE_DIR}/Core/Include)
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/boost/include)

add_executable(${EXE_NAME} ${INCLUDE} ${SOURCE})

#Boost.Asio support
target_link_libraries(${EXE_NAME} ${PROJECT_SOURCE_DIR}/ThirdParty/boost/lib/boost_system-vc140-mt-$<$<CONFIG:Debug>:gd->1_61.lib)
target_link_libraries(${EXE_NAME} ${PROJECT_SOURCE_DIR}/ThirdParty/boost/lib/boost_date_time-vc140-mt-$<$<CONFIG:Debug>:gd->1_61.lib)
target_link_libraries(${EXE_NAME} ${PROJECT_SOURCE_DIR}/ThirdParty/boost/lib/boost_regex-vc140-mt-$<$<CONFIG:Debug>:gd->1_61.lib)
target_link_libraries(${EXE_NAME} ${PROJECT_SOURCE_DIR}/ThirdParty/boost/lib/boost_coroutine-vc140-mt-$<$<CONFIG:Debug>:gd->1_61.lib)
#disable Boost autolinking
add_definitions("-DBOOST_ALL_NO_LIB")
#disable warning for WIN32_WINNT in boost
add_definitions("-D_WIN32_WINNT=0x0501")
