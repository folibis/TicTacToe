#
# tictactoe
# ruslan@muhlinin.com
# October 6, 2021
#

cmake_minimum_required(VERSION 3.5)

project(tictactoe)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)



