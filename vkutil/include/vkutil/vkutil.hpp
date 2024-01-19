//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

#include <span>
#include <filesystem>

#include "CommandRecorder.hpp"

import vulkan_hpp;

#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

namespace vkutil {
    [[nodiscard]] vk::ShaderModule createShaderModule(vk::Device device, std::span<const std::byte> source);
    [[nodiscard]] vk::ShaderModule createShaderModule(vk::Device device, const std::filesystem::path &path);
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device, std::span<const std::byte> source);
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device, const std::filesystem::path &path);

    [[nodiscard]] vk::ImageView createImageView(vk::Device device, vk::Image image, vk::ImageAspectFlags aspect, vk::Format format);
    [[nodiscard]] vk::raii::ImageView createImageView(const vk::raii::Device &device, vk::Image image, vk::ImageAspectFlags aspect, vk::Format format);

    template <typename Self, typename Result, typename ...Args>
    [[nodiscard]] Result executeSingleCommand(vk::Device device, vk::CommandPool commandPool, vk::Queue queue, const CommandRecorder<Self, Result, Args...> &commandRecorder, auto &&...args) {
        const vk::CommandBufferAllocateInfo allocateInfo {
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            1
        };
        const vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocateInfo)[0];

        constexpr vk::CommandBufferBeginInfo beginInfo {
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };
        commandBuffer.begin(beginInfo);

        if constexpr (std::is_void_v<Result>) {
            commandRecorder.recordCommand(commandBuffer, FWD(args)...);
            commandBuffer.end();

            const vk::SubmitInfo submitInfo {
                {},
                {},
                commandBuffer
            };
            queue.submit(submitInfo);
            queue.waitIdle();
        }
        else {
            Result result = commandRecorder.recordCommand(commandBuffer, FWD(args)...);
            commandBuffer.end();

            const vk::SubmitInfo submitInfo {
                {},
                {},
                commandBuffer
            };
            queue.submit(submitInfo);
            queue.waitIdle();

            return std::move(result);
        }
    }
}