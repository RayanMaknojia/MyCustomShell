cmake_minimum_required(VERSION 3.14)
project(ShellMaking VERSION 2.0 DESCRIPTION "This is simple shell making." LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(shell ${SOURCES} )
target_compile_options(shell PRIVATE -Wall -Wextra -g)

add_executable(lexdump tools/lexdump.c src/lexer.c)
target_include_directories(lexdump PRIVATE include)
target_compile_options(lexdump PRIVATE -Wall -Wextra -g)

add_executable(parsedump tools/parsedump.c src/lexer.c src/parser.c)
target_include_directories(parsedump PRIVATE include)
target_compile_options(parsedump PRIVATE -Wall -Wextra -g)
