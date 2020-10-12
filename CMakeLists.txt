cmake_minimum_required(VERSION 3.1)
project(ai4_engines)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(PERFT_SRCS ../src/Board.hpp ../src/perft.cpp)
add_executable(perft ${PERFT_SRCS})

set(RANDOM_SRCS ../src/Board.hpp ../src/UCIEngine.hpp ../src/random_engine.cpp)
add_executable(random_engine ${RANDOM_SRCS})
target_link_libraries(random_engine PRIVATE Threads::Threads)

set(ABP_SRCS ../src/Board.hpp ../src/UCIEngine.hpp ../src/abp_engine.cpp)
add_executable(abp_engine ${ABP_SRCS})
target_link_libraries(abp_engine PRIVATE Threads::Threads)

set(MINIMAX_SRCS ../src/Board.hpp ../src/UCIEngine.hpp ../src/minimax_engine.cpp)
add_executable(minimax_engine ${MINIMAX_SRCS})
target_link_libraries(minimax_engine PRIVATE Threads::Threads)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wshadow -pedantic -g -Og -lpthread")
#set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wshadow -pedantic -g -O3 -lpthread")
