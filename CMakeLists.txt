cmake_minimum_required(VERSION 3.13)
project(fixedbigint C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

add_executable(fixedbigint main.c bi32.c bi32.h lenstra.c lenstra.h bi63t.c bi63t.h)