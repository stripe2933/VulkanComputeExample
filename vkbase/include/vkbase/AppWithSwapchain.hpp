//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include "App.hpp"

import vulkan_hpp;

namespace vkbase{
    struct AppWithSwapchain : App{
        struct Swapchain {
            vk::raii::SwapchainKHR swapchain;
            vk::Format format;
            vk::ColorSpaceKHR colorSpace;
            vk::Extent2D extent;
            vk::PresentModeKHR presentMode;
        };

        vk::raii::SurfaceKHR surface;
        std::uint32_t presentQueueFamilyIndex;
        vk::Queue presentQueue;
        Swapchain swapchain;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::raii::ImageView> swapchainImageViews;

        void recreateSwapchain(vk::Format format, vk::ColorSpaceKHR colorSpace, vk::PresentModeKHR presentMode, vk::Extent2D extent);
    };
}
