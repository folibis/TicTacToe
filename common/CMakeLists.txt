#
# tictactoe common
# ruslan@muhlinin.com
# October 6, 2021
#

cmake_minimum_required(VERSION 3.5)

project(common C)

file(GLOB SRC . src/*.c)
file(GLOB INC . include/*.h)

add_library(${PROJECT_NAME} STATIC ${SRC} ${INC})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include  
)
