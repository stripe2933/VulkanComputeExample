//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include "Queues.hpp"

import vulkan_hpp;

namespace vkbase{
    struct App{
        vk::raii::Context context;
        vk::raii::Instance instance;
        vk::raii::PhysicalDevice physicalDevice;
        QueueFamilyIndices queueFamilyIndices;
        vk::raii::Device device;
        Queues queues;
    };
}