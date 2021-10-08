#
# tictactoe server
# ruslan@muhlinin.com
# October 6, 2021
#

cmake_minimum_required(VERSION 3.5)

project(server C)

file(GLOB SRC . src/*.c)
file(GLOB INC . include/*.h)

add_executable(${PROJECT_NAME} ${SRC} ${INC})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE -pthread)

add_dependencies(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME} PRIVATE common)

set(WITH_DOCS OFF CACHE BOOL "")
set(BUILD_TESTS OFF CACHE BOOL "")
set(ENABLE_WS OFF CACHE BOOL "")
set(WITH_TLS OFF CACHE BOOL "")
set(WITH_LIBSODIUM OFF CACHE BOOL "")
add_subdirectory(external/libzmq)
target_link_libraries(${PROJECT_NAME} PRIVATE libzmq)
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../external/libzmq/include
)
