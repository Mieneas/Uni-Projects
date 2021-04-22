cmake_minimum_required(VERSION 3.10)
project(Block2)

set(CMAKE_C_STANDARD 11)

add_executable(server server.c lib/server_functions.c lib/server_loop.c lib/text_file.c)
add_executable(client client.c)