cmake_minimum_required(VERSION 3.15)
project(minigit CXX)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Include directory
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/repository.cpp
    src/object.cpp
    src/index.cpp
    src/commands.cpp
)

# Target executable & linking
add_executable(minigit ${SOURCES})
target_link_libraries(minigit PRIVATE OpenSSL::Crypto ZLIB::ZLIB)