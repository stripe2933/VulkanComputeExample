//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include <functional>

#include "AppBuilder.hpp"
#include "AppWithSwapchain.hpp"

namespace vkbase{
    template <typename... DevicePNexts>
    struct AppWithSwapchainBuilder{
        AppBuilder<DevicePNexts...> appBuilder;
        vk::Format swapchainFormat = vk::Format::eB8G8R8A8Srgb;
        vk::ColorSpaceKHR swapchainColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

        [[nodiscard]] AppWithSwapchain build(const vk::ApplicationInfo &appInfo, std::function<vk::SurfaceKHR(vk::Instance)> surfaceFunc, vk::Extent2D extent);
    };

    // ------------------------------
    // Implementation
    // ------------------------------

    template <typename ... DevicePNexts>
    AppWithSwapchain AppWithSwapchainBuilder<DevicePNexts...>::build(
        const vk::ApplicationInfo &appInfo,
        std::function<vk::SurfaceKHR(vk::Instance)> surfaceFunc,
        vk::Extent2D extent){
        present(appBuilder.deviceExtensions, "VK_KHR_swapchain", helper::svConstructor);

        vk::raii::Context context{};

        // Create instance.
        const vk::InstanceCreateInfo instanceCreateInfo {
            appBuilder.instanceCreateFlags,
            &appInfo,
            appBuilder.instanceLayers,
            appBuilder.instanceExtensions,
        };
        vk::raii::Instance instance { context, instanceCreateInfo };

        // Create surface;
        vk::raii::SurfaceKHR surface { instance, surfaceFunc(*instance) };

        // Create physical device.
        std::vector physicalDevices = instance.enumeratePhysicalDevices();
        assert(!physicalDevices.empty());
        vk::raii::PhysicalDevice physicalDevice = std::move(physicalDevices.front());

        // Find queue family indices.
        // 1. Requested by AppBuilder.
        const QueueFamilyIndices queueFamilyIndices { *physicalDevice };
        // 2. Present queue family index.
        const std::uint32_t presentQueueFamilyIndex = [&]() {
            const std::size_t queueFamiliesCount = physicalDevice.getQueueFamilyProperties().size();
            for (std::uint32_t index = 0; index < queueFamiliesCount; ++index) {
                if (physicalDevice.getSurfaceSupportKHR(index, *surface)) {
                    return index;
                }
            }

            throw std::runtime_error { "Present not supported" };
        }();

        // If presentQueueFamilyIndex not exist in queueFamilyIndices, add it.
        const std::vector uniqueQueueFamilyIndices = [&]() {
            std::vector indices = queueFamilyIndices.getUniqueIndices();
            if (std::ranges::find(indices, presentQueueFamilyIndex) == indices.end()) {
                indices.push_back(presentQueueFamilyIndex);
            }
            return std::move(indices);
        }();

        constexpr float queuePriority = 1.f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
        std::ranges::transform(uniqueQueueFamilyIndices, back_inserter(queueCreateInfos), [=](std::uint32_t queueFamilyIndex) {
            return vk::DeviceQueueCreateInfo {
                {},
                queueFamilyIndex,
                1,
                &queuePriority,
            };
        });

        // Create device with queueCreateInfos, also chain pNexts.
        const vk::StructureChain deviceCreateInfoChain{
            vk::DeviceCreateInfo{
                {},
                queueCreateInfos,
                {},
                appBuilder.deviceExtensions,
            },
            get<DevicePNexts>(appBuilder.devicePNexts)...
        };
        vk::raii::Device device { physicalDevice, deviceCreateInfoChain.template get<vk::DeviceCreateInfo>() };

        // Get queues from device.
        const Queues queues { *device, queueFamilyIndices };
        const vk::Queue presentQueue = (*device).getQueue(presentQueueFamilyIndex, 0);

        // Create swapchain.
        const vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
        const std::uint32_t imageCount = std::max(capabilities.minImageCount + 1, capabilities.maxImageCount);

        const vk::SwapchainCreateInfoKHR createInfo {
            {},
            *surface,
            imageCount,
            swapchainFormat,
            swapchainColorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            {},
            capabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            swapchainPresentMode,
        };
        vk::raii::SwapchainKHR swapchain { device, createInfo };

        std::vector images = swapchain.getImages();
        std::vector<vk::raii::ImageView> imageViews;
        imageViews.reserve(images.size());
        std::ranges::transform(images, back_inserter(imageViews), [this, &device](vk::Image image) {
            const vk::ImageViewCreateInfo createInfo {
                {},
                image,
                vk::ImageViewType::e2D,
                swapchainFormat,
                {},
                { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
            };
            return vk::raii::ImageView { device, createInfo };
        });

        return {
            {
                std::move(context),
               std::move(instance),
               std::move(physicalDevice),
               queueFamilyIndices,
               std::move(device),
               queues
            },
            std::move(surface),
            presentQueueFamilyIndex,
            presentQueue,
            { std::move(swapchain), swapchainFormat, swapchainColorSpace, extent, swapchainPresentMode },
            std::move(images),
            std::move(imageViews),
        };
    }
}
