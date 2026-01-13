cmake_minimum_required(VERSION 3.14)
project(lob_market_maker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable
add_executable(lob_simulation ${SOURCES})

# Enable testing if needed later
# enable_testing()
