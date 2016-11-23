cmake_minimum_required(VERSION 3.6)
project(riscv_sim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -O2")

set(SOURCE_FILES main.cpp elf.cpp elf.h debug.h vmm.h decode.cpp register.h vmm.cpp syscall.cpp syscall.h)
add_executable(riscv_sim ${SOURCE_FILES})