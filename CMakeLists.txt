cmake_minimum_required(VERSION 3.14)
project(ShellMaking VERSION 1.0 DESCRIPTION "This is simple shell making." LANGUAGES C CXX)


# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP false
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(include)
file(GLOB SOURCES "src/*.c")
add_executable(shell_test gtest/shell_test.cpp)
add_executable(shell ${SOURCES} )

target_link_libraries(shell_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(shell_test)
