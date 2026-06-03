cmake_minimum_required(VERSION 3.10)
project(matmul C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra")

find_package(Threads REQUIRED)

add_executable(matmul
    src/main.c
    src/seq.c
    src/par.c
)

target_link_libraries(matmul Threads::Threads)
