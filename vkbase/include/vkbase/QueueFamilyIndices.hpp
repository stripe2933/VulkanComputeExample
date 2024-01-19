//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include <vector>

import vulkan_hpp;

namespace vkbase {
    struct QueueFamilyIndices {
        std::uint32_t graphics, compute;

        QueueFamilyIndices(vk::PhysicalDevice physicalDevice);
        QueueFamilyIndices(const QueueFamilyIndices&) = default;
        QueueFamilyIndices &operator=(const QueueFamilyIndices&) = default;

        [[nodiscard]] std::vector<std::uint32_t> getUniqueIndices() const noexcept;
    };
}