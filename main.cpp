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

// Multiply 2 to each PaddedFloats in the given storage buffer.
// Use ::recordCommand(vk::CommandBuffer) to record compute dispatch command.
class TwoMultiplier : public vkutil::CommandRecorder<TwoMultiplier> {
    const vkbase::App &vulkan;
    const vma::Allocator &allocator;
    const vk::Buffer &buffer;
    std::size_t numCount;

    vk::raii::DescriptorSetLayout storageBufferLayout = getDescriptorSetLayout();
    vk::raii::PipelineLayout pipelineLayout = createPipelineLayout(*storageBufferLayout);
    vk::raii::Pipeline computePipeline = createPipeline(*pipelineLayout);
    vk::raii::DescriptorPool descriptorPool = createDescriptorPool();
    vk::DescriptorSet descriptorSet = getDescriptorSet(*descriptorPool);

    [[nodiscard]] vk::raii::DescriptorSetLayout getDescriptorSetLayout() const {
        constexpr vk::DescriptorSetLayoutBinding layoutBinding {
            0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eCompute
        };
        const vk::DescriptorSetLayoutCreateInfo createInfo {
            {}, layoutBinding
        };
        return { vulkan.device, createInfo };
    }

    [[nodiscard]] vk::raii::PipelineLayout createPipelineLayout(vk::DescriptorSetLayout descriptorSetLayout) const {
        const vk::PipelineLayoutCreateInfo createInfo {
            {}, descriptorSetLayout
        };
        return { vulkan.device, createInfo };
    }

    [[nodiscard]] vk::raii::Pipeline createPipeline(vk::PipelineLayout pipelineLayout) const {
        const vk::raii::ShaderModule shaderModule
            = vkutil::createShaderModule(vulkan.device, "shaders/comp.comp.spv");
        const vk::PipelineShaderStageCreateInfo stageCreateInfo {
            {}, vk::ShaderStageFlagBits::eCompute, *shaderModule, "main"
        };
        const vk::ComputePipelineCreateInfo createInfo {
            {}, stageCreateInfo, pipelineLayout
        };
        return { vulkan.device, nullptr, createInfo };
    }

    [[nodiscard]] vk::raii::DescriptorPool createDescriptorPool() const {
        constexpr vk::DescriptorPoolSize poolSize {
            vk::DescriptorType::eStorageBuffer, 1
        };
        const vk::DescriptorPoolCreateInfo createInfo {
            {}, 1, poolSize
        };
        return { vulkan.device, createInfo };
    }

    [[nodiscard]] vk::DescriptorSet getDescriptorSet(vk::DescriptorPool descriptorPool) const {
        const vk::DescriptorSetAllocateInfo allocInfo {
            descriptorPool, *storageBufferLayout
        };
        return (*vulkan.device).allocateDescriptorSets(allocInfo)[0];
    }

public:
    TwoMultiplier(const vkbase::App &vulkan, const vma::Allocator &allocator, const vk::Buffer &buffer, std::size_t numCount)
        : vulkan { vulkan }, allocator { allocator }, buffer { buffer }, numCount { numCount } {
    }

    void recordCommand(vk::CommandBuffer commandBuffer) const {
        // Update descriptor set.
        const vk::DescriptorBufferInfo descriptorBufferInfo {
            buffer, 0, sizeof(PaddedFloat) * numCount
        };
        const vk::WriteDescriptorSet writeDescriptorSet {
            descriptorSet, 0, 0, 1, vk::DescriptorType::eStorageBuffer, {}, &descriptorBufferInfo
        };
        vulkan.device.updateDescriptorSets(writeDescriptorSet, {});

        // Dispatch compute shader.
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, *computePipeline);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, *pipelineLayout, 0, descriptorSet, {});
        commandBuffer.dispatch(numCount / 32 /* local_size_x in compute shader */, 1, 1);
    }
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

    // Create command pool.
    const vk::raii::CommandPool commandPool = [&] -> vk::raii::CommandPool {
        const vk::CommandPoolCreateInfo createInfo {
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer, vulkan.queueFamilyIndices.compute
        };
        return { vulkan.device, createInfo };
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

        // Print the original data before calculation. It would be [0, 1, ..., 127].
        std::println("{}", std::span { reinterpret_cast<const PaddedFloat*>(buffer.data), NUM_COUNT } | std::views::transform(&PaddedFloat::data));

        // Using vkutil::executeSingleCommand to get a command buffer from command pool, record command, submit and execute at once.
        const TwoMultiplier twoMultiplier { vulkan, allocator, buffer.buffer, NUM_COUNT };
        executeSingleCommand(*vulkan.device, *commandPool, vulkan.queues.compute, twoMultiplier);

        // Now let's check the result. It would be [0, 2, ..., 254].
        std::println("{}", std::span { reinterpret_cast<const PaddedFloat*>(buffer.data), NUM_COUNT } | std::views::transform(&PaddedFloat::data));
    }

    // Since VMA stuffs are not RAII, they have to be manually destroyed.
    allocator.destroy();

    // Wait until the device is idle. Happy Vulkan!
    vulkan.device.waitIdle();
}
