cmake_minimum_required(VERSION 3.19)
project(SS_Lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1 main.c script_mode.c list_mode.c fat32_lib.c utils.c)
