project(vkbase)

# --------------------
# Fetching dependencies.
# --------------------

find_package(Vulkan REQUIRED)

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

# --------------------
# vkbase library
# --------------------

add_library(vkbase
    src/QueueFamilyIndices.cpp
    src/Queues.cpp
    src/AppWithSwapchain.cpp
)
target_include_directories(vkbase PUBLIC include)
target_link_libraries(vkbase PUBLIC VulkanHppModule)

add_library(vkbase_module)
add_library(vkbase::module ALIAS vkbase_module)
target_sources(vkbase_module PUBLIC
    FILE_SET CXX_MODULES
    FILES src/vkbase.cppm
)
target_link_libraries(vkbase_module PUBLIC vkbase)