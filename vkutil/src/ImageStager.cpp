//
// Created by gomkyung2 on 1/5/24.
//

#include <vkutil/ImageStager.hpp>

#include <algorithm>

vkutil::ImageStager::ImageStager(vma::Allocator allocator): allocator { allocator } {
}

std::pair<vkutil::PersistentMappedBuffer, vkutil::Image> vkutil::ImageStager::recordCommand(
    vk::CommandBuffer commandBuffer,
    vk::Extent3D extent,
    std::span<const std::byte> data,
    vk::Format format) const {
    // Create staging buffer.
    const vk::BufferCreateInfo hostBufferCreateInfo {
        {},
        data.size_bytes(),
        vk::BufferUsageFlagBits::eTransferSrc
    };
    PersistentMappedBuffer hostBuffer { allocator, hostBufferCreateInfo };

    // Copy given data into stage buffer.
    std::ranges::copy(data, hostBuffer.data);

    // Create final result image in device local memory.
    const vk::ImageCreateInfo imageCreateInfo {
        {},
        vk::ImageType::e2D,
        format,
        extent,
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
    };
    constexpr vma::AllocationCreateInfo imageAllocInfo {
        vma::AllocationCreateFlagBits::eDedicatedMemory,
        vma::MemoryUsage::eAuto
    };
    Image deviceImage { allocator, imageCreateInfo, imageAllocInfo };

    // Change image layout for optimal to be transfer destination.
    const vk::ImageMemoryBarrier transitionTransferDstBarrier {
        {},
        vk::AccessFlagBits::eTransferWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        {}, // TODO
        {}, // TODO
        deviceImage.image,
        { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    };
    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        transitionTransferDstBarrier
    );

    // Copy host buffer data to image.
    const vk::BufferImageCopy copyRegion {
        0,
        0, 0,
        { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
        { 0, 0, 0 },
        extent
    };
    commandBuffer.copyBufferToImage(hostBuffer.buffer, deviceImage.image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

    return { std::move(hostBuffer), std::move(deviceImage) };
}
