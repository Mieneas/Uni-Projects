cmake_minimum_required(VERSION 3.5.1)
project(Block4 C)

set(CMAKE_C_STANDARD 99)

add_executable(Block4-peer peer.c src/server.c src/packet.c src/util.c src/hash_table.c src/neighbour.c include/neighbour.h include/requests.h src/requests.c)
target_include_directories(Block4-peer PRIVATE include)

set_target_properties(Block4-peer PROPERTIES OUTPUT_NAME "peer")
