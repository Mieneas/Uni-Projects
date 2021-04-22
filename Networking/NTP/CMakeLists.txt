cmake_minimum_required(VERSION 3.5.1)
project(Block5 C)

set(CMAKE_C_STANDARD 99)

add_executable(ntpclient main_client.c src/client.c  src/packet.c src/util.c src/network.c)

target_include_directories(ntpclient PRIVATE include)

set_target_properties(ntpclient PROPERTIES OUTPUT_NAME "ntpclient")