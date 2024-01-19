//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

#include <span>

#include "CommandRecorder.hpp"
#include "PersistentMappedBuffer.hpp"

import vk_mem_alloc_hpp;

namespace vkutil {
    class BufferStager : public CommandRecorder<BufferStager, std::pair<PersistentMappedBuffer, Buffer>, std::span<const std::byte>, vk::BufferUsageFlags> {
        vma::Allocator allocator;

    public:
        explicit BufferStager(vma::Allocator allocator);

        std::pair<PersistentMappedBuffer, Buffer> recordCommand(vk::CommandBuffer commandBuffer, std::span<const std::byte> data, vk::BufferUsageFlags usage) const;
    };
}