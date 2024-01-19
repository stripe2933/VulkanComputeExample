//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

import vulkan_hpp;
import vk_mem_alloc_hpp;

namespace vkutil{
    class Buffer {
    protected:
        vma::Allocator allocator;

    public:
        vk::Buffer buffer;
        vma::Allocation allocation;

        Buffer(vma::Allocator allocator, const vk::BufferCreateInfo &createInfo, const vma::AllocationCreateInfo &allocInfo);
        Buffer(const Buffer&) = delete;
        Buffer(Buffer &&) noexcept;
        Buffer &operator=(const Buffer&) = delete;
        Buffer &operator=(Buffer &&) noexcept;
        virtual ~Buffer();
    };
}
