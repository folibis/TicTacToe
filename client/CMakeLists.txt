#
# tictactoe client
# ruslan@muhlinin.com
# October 6, 2021
#

cmake_minimum_required(VERSION 3.5)

project(client C)


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

