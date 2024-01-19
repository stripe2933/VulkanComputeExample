//
// Created by gomkyung2 on 1/19/24.
//

import std;
import vkbase;
import vkutil;

// Use 16-byte aligned struct, because Vulkan requires all storage buffer to be 16-byte aligned.
struct alignas(16) PaddedFloat {
    float data;
};

int main() {
    constexpr vk::ApplicationInfo appInfo {
        "Vulkan Compute Example", 0,
        nullptr, 0,
        vk::makeApiVersion(0, 1, 2, 0)
    };
    const vkbase::App vulkan = vkbase::AppBuilder{}
#if __APPLE__
        .enablePotability()
#endif
#ifndef NDEBUG
        .enableValidationLayers()
#endif
        .build(appInfo);

    // Create descriptor set layout for storage buffer.
    constexpr vk::DescriptorSetLayoutBinding storageBufferLayoutBinding {
        0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute
    };
    const vk::DescriptorSetLayoutCreateInfo storageBufferLayoutCreateInfo {
        {}, storageBufferLayoutBinding
    };
    const vk::raii::DescriptorSetLayout storageBufferLayout { vulkan.device, storageBufferLayoutCreateInfo };

    // Create compute pipeline layout.
    const vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo {
        {}, *storageBufferLayout
    };
    const vk::raii::PipelineLayout pipelineLayout { vulkan.device, pipelineLayoutCreateInfo };

    // Create compute pipeline.
    const vk::raii::ShaderModule computeShaderModule
        = vkutil::createShaderModule(vulkan.device, "shaders/comp.comp.spv");
    const vk::PipelineShaderStageCreateInfo computeShaderStageCreateInfo {
        {}, vk::ShaderStageFlagBits::eCompute, *computeShaderModule, "main"
    };
    const vk::ComputePipelineCreateInfo computePipelineCreateInfo {
        {}, computeShaderStageCreateInfo, *pipelineLayout
    };
    const vk::raii::Pipeline computePipeline { vulkan.device, nullptr, computePipelineCreateInfo };

    // Create command pool.
    const vk::CommandPoolCreateInfo commandPoolCreateInfo {
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, vulkan.queueFamilyIndices.compute
    };
    const vk::raii::CommandPool commandPool { vulkan.device, commandPoolCreateInfo };

    // Create descriptor pool.
    constexpr vk::DescriptorPoolSize descriptorPoolSize {
        vk::DescriptorType::eStorageBuffer, 1
    };
    const vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo {
        {}, 1, descriptorPoolSize
    };
    const vk::raii::DescriptorPool descriptorPool { vulkan.device, descriptorPoolCreateInfo };

    // Get descriptor set from descriptor pool.
    const vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo {
        *descriptorPool, *storageBufferLayout
    };
    const vk::DescriptorSet descriptorSet = (*vulkan.device).allocateDescriptorSets(descriptorSetAllocateInfo)[0];

    // Create VMA allocator.
    const vma::AllocatorCreateInfo allocatorCreateInfo {
        {},
        *vulkan.physicalDevice,
        *vulkan.device,
        {}, {}, {}, {}, {},
        *vulkan.instance,
        appInfo.apiVersion,
    };
    const vma::Allocator allocator = createAllocator(allocatorCreateInfo);

    // Start new scope, because all buffers allocated by VMA must be destroyed before the allocator is destroyed.
    {
        // As mentioned above, use 16-byte aligned struct because Vulkan requires all storage buffer to be 16-byte aligned.
        constexpr std::size_t NUM_COUNT = 128;
        const std::vector<PaddedFloat> nums { std::from_range,
            std::views::iota(0UZ, NUM_COUNT) // [0, 1, ..., 127]
            | std::views::transform([](std::size_t x) { return PaddedFloat { static_cast<float>(x) }; }) // convert them to PaddedFloat.
        };

        // Create storage buffer.
        constexpr vk::BufferCreateInfo bufferCreateInfo {
            {}, sizeof(PaddedFloat) * NUM_COUNT, vk::BufferUsageFlagBits::eStorageBuffer
        };
        const vkutil::PersistentMappedBuffer buffer { allocator, bufferCreateInfo };
        std::ranges::copy(as_bytes(std::span { nums }), buffer.data);

        // Print the original data before calculation. It would be [0, 1, ..., 128].
        std::println("{}", std::span { reinterpret_cast<const PaddedFloat*>(buffer.data), NUM_COUNT } | std::views::transform(&PaddedFloat::data));

        // Update descriptor set.
        const vk::DescriptorBufferInfo descriptorBufferInfo {
            buffer.buffer, 0, sizeof(PaddedFloat) * NUM_COUNT
        };
        const vk::WriteDescriptorSet writeDescriptorSet {
            descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, {}, &descriptorBufferInfo
        };
        vulkan.device.updateDescriptorSets(writeDescriptorSet, {});

        // Get command buffer from command pool.
        const vk::CommandBufferAllocateInfo allocateInfo {
            *commandPool, vk::CommandBufferLevel::ePrimary, 1
        };
        const vk::CommandBuffer commandBuffer = (*vulkan.device).allocateCommandBuffers(allocateInfo)[0];

        // Dispatch compute shader.
        constexpr vk::CommandBufferBeginInfo beginInfo {
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };
        commandBuffer.begin(beginInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, *computePipeline);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, *pipelineLayout, 0, descriptorSet, {});
        commandBuffer.dispatch(NUM_COUNT / 32 /* local_size_x in compute shader */, 1, 1);
        commandBuffer.end();

        // Submit command buffer to queue.
        const vk::SubmitInfo submitInfo {
            {}, {}, commandBuffer
        };
        vulkan.queues.compute.submit(submitInfo);
        vulkan.queues.compute.waitIdle();

        // Now let's check the result. It would be [0, 2, ..., 254].
        std::println("{}", std::span { reinterpret_cast<const PaddedFloat*>(buffer.data), NUM_COUNT } | std::views::transform(&PaddedFloat::data));
    }

    // Since VMA stuffs are not RAII, they have to be manually destroyed.
    allocator.destroy();

    // Wait until the device is idle. Happy Vulkan!
    vulkan.device.waitIdle();
}
