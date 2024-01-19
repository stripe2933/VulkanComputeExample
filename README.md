# Vulkan Compute Example

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Easy Vulkan compute example with easy-to-use bootstrapper and utils (about 160 LoC).

If you want to see the OOP style for same function, it would be worth to see [OOP style branch](https://github.com/stripe2933/VulkanComputeExample/tree/oop-style).

```c++
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
    const vk::raii::DescriptorSetLayout storageBufferLayout = [&] -> vk::raii::DescriptorSetLayout {
        constexpr vk::DescriptorSetLayoutBinding layoutBinding {
            0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute
        };
        const vk::DescriptorSetLayoutCreateInfo createInfo {
            {}, layoutBinding
        };
        return { vulkan.device, createInfo };
    }();

    // Create compute pipeline layout.
    const vk::raii::PipelineLayout pipelineLayout = [&] -> vk::raii::PipelineLayout {
        const vk::PipelineLayoutCreateInfo createInfo {
            {}, *storageBufferLayout
        };
        return { vulkan.device, createInfo };
    }();

    // Create compute pipeline.
    const vk::raii::Pipeline computePipeline = [&] -> vk::raii::Pipeline {
        const vk::raii::ShaderModule shaderModule
            = vkutil::createShaderModule(vulkan.device, "shaders/comp.comp.spv");
        const vk::PipelineShaderStageCreateInfo stageCreateInfo {
            {}, vk::ShaderStageFlagBits::eCompute, *shaderModule, "main"
        };
        const vk::ComputePipelineCreateInfo createInfo {
            {}, stageCreateInfo, *pipelineLayout
        };
        return { vulkan.device, nullptr, createInfo };
    }();

    // Create command pool.
    const vk::raii::CommandPool commandPool = [&] -> vk::raii::CommandPool {
        const vk::CommandPoolCreateInfo createInfo {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer, vulkan.queueFamilyIndices.compute
        };
        return { vulkan.device, createInfo };
    }();

    // Create descriptor pool.
    const vk::raii::DescriptorPool descriptorPool = [&] -> vk::raii::DescriptorPool {
        constexpr vk::DescriptorPoolSize poolSize {
            vk::DescriptorType::eStorageBuffer, 1
        };
        const vk::DescriptorPoolCreateInfo createInfo {
            {}, 1, poolSize
        };
        return { vulkan.device, createInfo };
    }();

    // Get descriptor set from descriptor pool.
    const vk::DescriptorSet descriptorSet = [&] {
        const vk::DescriptorSetAllocateInfo allocInfo {
            *descriptorPool, *storageBufferLayout
        };
        return (*vulkan.device).allocateDescriptorSets(allocInfo)[0];
    }();

    // Create VMA allocator.
    const vma::Allocator allocator = [&] {
        const vma::AllocatorCreateInfo createInfo {
            {},
            *vulkan.physicalDevice,
            *vulkan.device,
            {}, {}, {}, {}, {},
            *vulkan.instance,
            appInfo.apiVersion,
        };
        return createAllocator(createInfo);
    }();

    // Start new scope, because all buffers allocated by VMA must be destroyed before the allocator is destroyed.
    {
        // As mentioned above, use 16-byte aligned struct because Vulkan requires all storage buffer to be 16-byte aligned.
        constexpr std::size_t NUM_COUNT = 128;
        const std::vector<PaddedFloat> nums { std::from_range,
            std::views::iota(0UZ, NUM_COUNT) // [0, 1, ..., 127]
            | std::views::transform([](std::size_t x) { return PaddedFloat { static_cast<float>(x) }; }) // convert them to PaddedFloat.
        };

        // Create storage buffer.
        const vkutil::PersistentMappedBuffer buffer = [&] -> vkutil::PersistentMappedBuffer {
            constexpr vk::BufferCreateInfo createInfo {
                {}, sizeof(PaddedFloat) * NUM_COUNT, vk::BufferUsageFlagBits::eStorageBuffer
            };
            return { allocator, createInfo };
        }();
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
        const vk::CommandBuffer commandBuffer = [&] {
            const vk::CommandBufferAllocateInfo allocInfo {
                *commandPool, vk::CommandBufferLevel::ePrimary, 1
            };
            return (*vulkan.device).allocateCommandBuffers(allocInfo)[0];
        }();

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

```

**Output:**
```
[0, 1, 2, ..., 126, 127]
[0, 2, 4, ..., 252, 254]
```
