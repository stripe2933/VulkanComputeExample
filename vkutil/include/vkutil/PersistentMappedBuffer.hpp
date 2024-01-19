//
// Created by gomkyung2 on 1/13/24.
//

#pragma once

#include <cstddef>

#include "Buffer.hpp"

import vk_mem_alloc_hpp;

namespace vkutil{
    class PersistentMappedBuffer final : public Buffer {
        static constexpr vma::AllocationCreateInfo allocInfo {
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped,
            vma::MemoryUsage::eAutoPreferHost,
        };

    public:
        std::byte *data;

        PersistentMappedBuffer(vma::Allocator allocator, const vk::BufferCreateInfo &createInfo);
        PersistentMappedBuffer(const PersistentMappedBuffer&) = delete;
        PersistentMappedBuffer(PersistentMappedBuffer &&) noexcept;
        PersistentMappedBuffer &operator=(const PersistentMappedBuffer&) = delete;
        PersistentMappedBuffer &operator=(PersistentMappedBuffer &&) noexcept;
        ~PersistentMappedBuffer() override;
    };
}