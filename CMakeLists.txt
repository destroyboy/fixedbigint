cmake_minimum_required(VERSION 3.13)
project(fixedbigint C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")

add_executable(fixedbigint main.c bigint.c bigint.h lenstra.c lenstra.h)