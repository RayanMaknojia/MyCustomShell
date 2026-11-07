cmake_minimum_required(VERSION 3.14)
project(ShellMaking VERSION 2.0 DESCRIPTION "This is simple shell making." LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(shell ${SOURCES} )
target_compile_options(shell PRIVATE -Wall -Wextra -g)

