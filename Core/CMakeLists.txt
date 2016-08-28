set(LIB_NAME Core)

set(INCLUDE
    "Include/Core/Core.hpp"
    "Include/Core/Utility.hpp")
set(SOURCE
    "Src/Utility.cpp")

source_group("Include" FILES ${INCLUDE})
source_group("Source" FILES ${SOURCE})

include_directories(./Include)

add_library(${LIB_NAME} ${INCLUDE} ${SOURCE})