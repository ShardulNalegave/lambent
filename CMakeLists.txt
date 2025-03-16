cmake_minimum_required(VERSION 3.10)
project(lambent)

set(h_srcs "src/scanner.h")
set(c_srcs "src/main.c" "src/scanner.c")

add_executable(lambent ${h_srcs} ${c_srcs})