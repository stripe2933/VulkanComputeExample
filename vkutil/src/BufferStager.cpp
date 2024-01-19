//
// Created by gomkyung2 on 1/5/24.
//

#include <vkutil/BufferStager.hpp>

#include <algorithm>

vkutil::BufferStager::BufferStager(vma::Allocator allocator)
    : allocator { allocator } {
}

std::pair<vkutil::PersistentMappedBuffer, vkutil::Buffer> vkutil::BufferStager::recordCommand(
    vk::CommandBuffer commandBuffer,
    std::span<const std::byte> data,
    vk::BufferUsageFlags usage) const {
    // Create host buffer with given data size.
    const vk::BufferCreateInfo hostBufferCreateInfo {
        {},
        data.size_bytes(),
        vk::BufferUsageFlagBits::eTransferSrc,
    };
    PersistentMappedBuffer hostBuffer { allocator, hostBufferCreateInfo };

    // Copy given data into stage buffer.
    std::ranges::copy(data, hostBuffer.data);

    // Create device-local buffer with same size.
    const vk::BufferCreateInfo deviceBufferCreateInfo {
        {},
        data.size_bytes(),
        vk::BufferUsageFlagBits::eTransferDst | usage,
    };
    constexpr vma::AllocationCreateInfo deviceAllocInfo {
        vma::AllocationCreateFlagBits::eDedicatedMemory,
        vma::MemoryUsage::eAuto,
    };
    Buffer deviceBuffer { allocator, deviceBufferCreateInfo, deviceAllocInfo };

    // Record command.
    const vk::BufferCopy copyRegion { 0, 0, data.size_bytes() };
    commandBuffer.copyBuffer(hostBuffer.buffer, deviceBuffer.buffer, copyRegion);

    return { std::move(hostBuffer), std::move(deviceBuffer) };
}
