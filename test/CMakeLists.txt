
enable_testing()

FILE (GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(GMOCK_LIBRARY "E:/Github/gmock-1.7.0/msvc/2010/Release/gmock.lib" CACHE FILEPATH "Path to the (static) gmock library")
set(GMOCK_INCLUDE_DIR "E:/Github/gmock-1.7.0/include" CACHE PATH "Path to the gmock includes")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/)    #easy-erg
include_directories(${GMOCK_INCLUDE_DIR})                   #gmock
include_directories(${GMOCK_INCLUDE_DIR}/../gtest/include)  #gtest

add_executable(test_discover_ergs ${HEADERS} test_discover_ergs.cpp )
add_executable(test_running_average ${HEADERS} test_running_average.cpp )

target_link_libraries(test_discover_ergs    easy-erg
                                            ${GMOCK_LIBRARY})
target_link_libraries(test_running_average  easy-erg
                                            ${GMOCK_LIBRARY})
                                            
add_test(c2tests test_discover_ergs)
add_test(c2tests test_running_average)
