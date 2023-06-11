cmake_minimum_required(VERSION 3.14)

project(
    thinker-zero
    VERSION 0.1
    DESCRIPTION "A chess engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# Test what this is
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE SRC_FILES REQUIRE_DEPENDS "src/*.hpp" "src/*.cpp")

message(NOTICE "List of all sources: ${SRC_FILES}")

add_executable(thinker-zero ${SRC_FILES})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(thinker-zero PUBLIC "src")

target_compile_options(thinker-zero PRIVATE -Wall -Wextra -Wpedantic -Werror)