cmake_minimum_required(VERSION 3.16)
project(MonteCarloExchange VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)

add_library(mcx STATIC
  src/Random.cpp
  src/PriceModel.cpp
  src/Stock.cpp
  src/Index.cpp
  src/Market.cpp
  src/MarketEvent.cpp
)
target_include_directories(mcx PUBLIC include)
target_link_libraries(mcx PUBLIC Threads::Threads)
if(MSVC)
  target_compile_options(mcx PUBLIC /W4 /utf-8)
else()
  target_compile_options(mcx PUBLIC -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(mcx_tests tests/test_main.cpp)
target_link_libraries(mcx_tests PRIVATE mcx)
add_test(NAME unit_tests COMMAND mcx_tests)