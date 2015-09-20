cmake_minimum_required (VERSION 3.3.2)
project (easy-erg)

set (C2SDK "D:/Concept2/Software Development Kit" CACHE PATH "Sets the directory where the C2 SDK headers reside")
FILE(GLOB C2SDK_LIBRARIES ${C2SDK}/*.lib)
include_directories (${C2SDK})

add_subdirectory (src)
add_subdirectory (demo)
#add_subdirectory (test)
