cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TeaApp)

# Specify the C++ standard (optional, but recommended)
set(CMAKE_CXX_STANDARD 11)

# Include the current directory for header files (like tea.h and base64.h)
include_directories(${CMAKE_SOURCE_DIR})

# Add the executable target with main.cpp, tea.cpp, and base64.cpp as source files
add_executable(tea_app main.cpp)
