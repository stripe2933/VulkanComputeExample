//
// Created by gomkyung2 on 1/4/24.
//

module;

#include <vkbase/App.hpp>
#include <vkbase/AppBuilder.hpp>
#include <vkbase/AppWithSwapchain.hpp>
#include <vkbase/AppWithSwapchainBuilder.hpp>
#include <vkbase/QueueFamilyIndices.hpp>
#include <vkbase/Queues.hpp>

export module vkbase;
export import vulkan_hpp;

export namespace vkbase{
    using vkbase::App;
    using vkbase::AppBuilder;
    using vkbase::AppWithSwapchain;
    using vkbase::AppWithSwapchainBuilder;
    using vkbase::QueueFamilyIndices;
    using vkbase::Queues;
}