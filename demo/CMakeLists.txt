
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/)

add_executable(demo demo.cpp)
add_executable(demoDiscoverErgs demoDiscoverErgs.cpp)
add_executable(demoIntervals demoIntervals.cpp)
add_executable(demoState demoState.cpp)

target_link_libraries(demo easy-erg)
target_link_libraries(demoDiscoverErgs easy-erg)
target_link_libraries(demoIntervals easy-erg)
target_link_libraries(demoState easy-erg)
