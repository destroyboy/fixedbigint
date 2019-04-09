cmake_minimum_required(VERSION 3.13)
project(fixedbigint C)

set(CMAKE_C_STANDARD 99)

add_executable(fixedbigint main.c bigint.c bigint.h lenstra.c lenstra.h)