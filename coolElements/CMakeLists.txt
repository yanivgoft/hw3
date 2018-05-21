cmake_minimum_required(VERSION 3.10)
project(coolElements C)

set(CMAKE_C_STANDARD 99)

add_executable(coolElements main.c coolElement.c listFilter.c)