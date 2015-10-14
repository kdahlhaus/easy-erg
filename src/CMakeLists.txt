
FILE (GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE (GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library (easy-erg ${SOURCES} ${HEADERS})

target_link_libraries(easy-erg ${C2SDK_LIBRARIES})
