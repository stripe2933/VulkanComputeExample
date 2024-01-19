//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include "QueueFamilyIndices.hpp"

import vulkan_hpp;

namespace vkbase {
    struct Queues {
        vk::Queue graphics;
        vk::Queue compute;

        Queues(vk::Device device, QueueFamilyIndices queueFamilyIndices);
        Queues(const Queues&) = default;
        Queues &operator=(const Queues&) = default;
    };
}
