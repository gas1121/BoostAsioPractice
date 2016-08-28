set(LIB_NAME Core)

set(INCLUDE
    "Include/Core/Core.hpp"
    "Include/Core/RandomUtility.hpp")
set(SOURCE
    "Src/RandomUtility.cpp")

source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

include_directories(./Include/Core)

add_library(${LIB_NAME} ${INCLUDE} ${SOURCE})