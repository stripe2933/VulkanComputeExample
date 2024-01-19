project(vkutil)

option(USE_GLFW_WINDOW "Use GLFW window" OFF)

# --------------------
# Fetching dependencies.
# --------------------

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
CPMAddPackage("gh:stripe2933/VulkanMemoryAllocator-Hpp#1528f1a")

if (USE_GLFW_WINDOW)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
endif()

# --------------------
# Module configurations for dependencies.
# --------------------

if (NOT TARGET VulkanHppModule)
    # Require Vulkan version ≥ 1.3.256 (earliest version when the Vulkan module was available)
    if (${Vulkan_VERSION} VERSION_LESS "1.3.256")
        message(FATAL_ERROR "Minimum required Vulkan version for C++ modules is 1.3.256. Found ${Vulkan_VERSION}.")
    endif()

    # Setup Vulkan C++ module as a library
    add_library(VulkanHppModule)
    target_sources(VulkanHppModule PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${Vulkan_INCLUDE_DIR}
        FILES ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm
    )
    target_link_libraries(VulkanHppModule PUBLIC Vulkan::Vulkan)
    target_compile_definitions(VulkanHppModule PUBLIC
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SMART_HANDLE
        VULKAN_HPP_NO_TO_STRING
    )
endif()

if (NOT TARGET VulkanMemoryAllocator-Hpp_module)
    add_library(VulkanMemoryAllocator-Hpp_module)
    add_library(VulkanMemoryAllocator-Hpp::module ALIAS VulkanMemoryAllocator-Hpp_module)
    target_sources(VulkanMemoryAllocator-Hpp_module PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}
        FILES ${VulkanMemoryAllocator-Hpp_SOURCE_DIR}/src/vk_mem_alloc.cppm
    )
    target_link_libraries(VulkanMemoryAllocator-Hpp_module PUBLIC
        VulkanMemoryAllocator-Hpp
        VulkanHppModule
        GPUOpen::VulkanMemoryAllocator
    )
    target_compile_definitions(VulkanMemoryAllocator-Hpp_module PUBLIC
        VULKAN_HPP_NO_STRUCT_SETTERS
        VMA_VULKAN_VERSION=1002000
    )
endif()

# --------------------
# vkutil library
# --------------------

add_library(vkutil
    src/Buffer.cpp
    src/BufferStager.cpp
    src/Image.cpp
    src/ImageStager.cpp
    src/PersistentMappedBuffer.cpp
    src/vkutil.cpp
)
target_include_directories(vkutil PUBLIC include)
target_link_libraries(vkutil PUBLIC
    VulkanHppModule
    VulkanMemoryAllocator-Hpp::module
)

if (USE_GLFW_WINDOW)
    target_sources(vkutil PRIVATE src/GlfwWindow.cpp)
    target_compile_definitions(vkutil PUBLIC
        VKUTIL_USE_GLFW_WINDOW
        GLFW_INCLUDE_NONE
    )
    target_link_libraries(vkutil PUBLIC glfw glm::glm)
endif()

add_library(vkutil_module)
add_library(vkutil::module ALIAS vkutil_module)
target_sources(vkutil_module PUBLIC
    FILE_SET CXX_MODULES
    FILES src/vkutil.cppm
)
target_link_libraries(vkutil_module PUBLIC vkutil)