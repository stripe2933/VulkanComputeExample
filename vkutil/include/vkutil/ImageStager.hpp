//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

#include <span>

#include "PersistentMappedBuffer.hpp"
#include "CommandRecorder.hpp"
#include "Image.hpp"

namespace vkutil {
    class ImageStager : public CommandRecorder<ImageStager, std::pair<PersistentMappedBuffer, Image>, vk::Extent3D, std::span<const std::byte>, vk::Format> {
        vma::Allocator allocator;

    public:
        explicit ImageStager(vma::Allocator allocator);

        std::pair<PersistentMappedBuffer, Image> recordCommand(vk::CommandBuffer commandBuffer, vk::Extent3D extent, std::span<const std::byte> data, vk::Format format) const;
    };
}