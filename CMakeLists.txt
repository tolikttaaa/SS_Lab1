cmake_minimum_required(VERSION 3.19)
project(System-Software C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1 Lab1/main.c Lab1/script_mode.c Lab1/list_mode.c Lab1/fat32_lib.c Lab1/utils.c)