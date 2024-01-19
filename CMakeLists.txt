cmake_minimum_required(VERSION 3.28)
project(VulkanComputeExample)

set(CMAKE_CXX_STANDARD 23)

# --------------------
# Include CMake scripts.
# --------------------

include(cmake/EnableStandardLibraryModule.cmake)
include(cmake/InstallCPM.cmake)
include(cmake/CompileVulkanShader.cmake)

# --------------------
# Include CMake subdirectories.
# --------------------

add_subdirectory(vkbase)
add_subdirectory(vkutil)

# --------------------
# Library dependencies.
# --------------------

find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)

# --------------------
# Main executable.
# --------------------

add_executable(VulkanComputeExample main.cpp)
target_link_libraries(VulkanComputeExample PRIVATE
    vkbase::module
    vkutil::module
)

# --------------------
# Vulkan shader compilation.
# --------------------

compile_shader(VulkanComputeExample
    FORMAT spv
    SOURCES shaders/comp.comp
)