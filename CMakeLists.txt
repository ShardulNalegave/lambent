cmake_minimum_required(VERSION 3.15)
project(lambent LANGUAGES C VERSION 0.1.0)

set(LAMBENT_SOURCES
    "src/main.c")

add_executable(lambent ${LAMBENT_SOURCES})
target_include_directories(lambent
    PRIVATE
        "src/include"
)
