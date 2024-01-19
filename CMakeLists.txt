cmake_minimum_required(VERSION 3.28)
project(CppStandardLibraryModule)

set(CMAKE_CXX_STANDARD 23)

# --------------------
# Include CMake scripts.
# --------------------

include(cmake/EnableStandardLibraryModule.cmake)

# --------------------
# Main executable.
# --------------------

add_executable(CppStandardLibraryModule main.cpp)