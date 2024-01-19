//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include <cassert>
#include <algorithm>
#include <functional>
#include <vector>

#include "App.hpp"

import vulkan_hpp;

namespace vkbase {
    template <typename... DevicePNexts>
    struct AppBuilder {
        struct DefaultPhysicalDeviceRater {
            [[nodiscard]] std::uint32_t operator()(vk::PhysicalDevice physicalDevice) const;
        };

        vk::InstanceCreateFlags instanceCreateFlags{};
        std::vector<const char*> instanceLayers{};
        std::vector<const char*> instanceExtensions{};
        std::function<std::uint32_t(vk::PhysicalDevice)> physicalDeviceRater = DefaultPhysicalDeviceRater{};
        vk::PhysicalDeviceFeatures physicalDeviceFeatures{};
        std::vector<const char*> deviceExtensions{};
        std::tuple<DevicePNexts...> devicePNexts{};

        AppBuilder &enableValidationLayers();
        AppBuilder &enablePotability();
        [[nodiscard]] App build(const vk::ApplicationInfo &appInfo);
    };

    // ------------------------------
    // Implementation
    // ------------------------------

namespace helper {
    template <typename T, typename V, typename P = std::identity>
    void present(std::vector<T> &container, V &&value, P &&proj = {}) {
        if (std::ranges::find(container, value, std::forward<P>(proj)) == container.end()) {
            container.emplace_back(std::forward<V>(value));
        }
    }

    struct SVConstructor {
        template <typename T>
        constexpr std::string_view operator()(T &&x) const noexcept{
            return { std::forward<T>(x) };
        }
    };
    inline constexpr SVConstructor svConstructor{};
}

    template <typename ... DevicePNexts>
    std::uint32_t AppBuilder<DevicePNexts...>::DefaultPhysicalDeviceRater::operator()(vk::PhysicalDevice physicalDevice) const {
        // Check if given device supports the required queue families.
        try {
            const QueueFamilyIndices queueFamilyIndices { physicalDevice }; (void)queueFamilyIndices;
        }
        catch (const std::runtime_error&) {
            return 0;
        }

        std::uint32_t score = 0;

        // Rate physical device by its ability.
        const vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            score += 1000;
        }
        score += properties.limits.maxImageDimension2D;

        return score;
    }

    template <typename ... DevicePNexts>
    AppBuilder<DevicePNexts...> & AppBuilder<DevicePNexts...>::enableValidationLayers() {
        present(instanceLayers, "VK_LAYER_KHRONOS_validation", helper::svConstructor);
        return *this;
    }

    template <typename ... DevicePNexts>
    AppBuilder<DevicePNexts...> & AppBuilder<DevicePNexts...>::enablePotability() {
        instanceCreateFlags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
        present(instanceExtensions, "VK_KHR_portability_enumeration", helper::svConstructor);
        present(instanceExtensions, "VK_KHR_get_physical_device_properties2", helper::svConstructor);
        present(deviceExtensions, "VK_KHR_portability_subset", helper::svConstructor);
        return *this;
    }

    template <typename ... DevicePNexts>
    App AppBuilder<DevicePNexts...>::build(const vk::ApplicationInfo &appInfo) {
        vk::raii::Context context{};

        const vk::InstanceCreateInfo instanceCreateInfo {
            instanceCreateFlags,
            &appInfo,
            instanceLayers,
            instanceExtensions,
        };
        vk::raii::Instance instance { context, instanceCreateInfo };

        std::vector physicalDevices = instance.enumeratePhysicalDevices();
        assert(!physicalDevices.empty());
        vk::raii::PhysicalDevice physicalDevice = [&]() {
            auto it = std::ranges::max_element(physicalDevices, {}, [this](const vk::raii::PhysicalDevice &physicalDevice) {
                return physicalDeviceRater(*physicalDevice);
            });
            if (physicalDeviceRater(**it) == 0) {
                throw std::runtime_error { "No physical device for required configuration." };
            }

            return *it;
        }();

        const QueueFamilyIndices queueFamilyIndices { *physicalDevice };

        constexpr float queuePriority = 1.f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::ranges::transform(queueFamilyIndices.getUniqueIndices(), back_inserter(queueCreateInfos), [=](std::uint32_t queueFamilyIndex) {
            return vk::DeviceQueueCreateInfo {
                {},
                queueFamilyIndex,
                1,
                &queuePriority,
            };
        });
        const vk::StructureChain deviceCreateInfoChain{
            vk::DeviceCreateInfo{
                {},
                queueCreateInfos,
                {},
                deviceExtensions,
                &physicalDeviceFeatures,
            },
            get<DevicePNexts>(devicePNexts)...
        };
        vk::raii::Device device { physicalDevice, deviceCreateInfoChain.get() };

        const Queues queues { *device, queueFamilyIndices };

        return {
            std::move(context),
            std::move(instance),
            std::move(physicalDevice),
            queueFamilyIndices,
            std::move(device),
            queues
        };
    }
}