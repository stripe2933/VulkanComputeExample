//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

import vulkan_hpp;
import vk_mem_alloc_hpp;

namespace vkutil {
    class Image {
        vma::Allocator allocator;

    public:
        vk::Image image;
        vma::Allocation allocation;

        Image(vma::Allocator allocator, const vk::ImageCreateInfo &createInfo, const vma::AllocationCreateInfo &allocInfo);
        Image(const Image&) = delete;
        Image(Image &&src) noexcept;
        Image &operator=(const Image&) = delete;
        Image &operator=(Image &&src) noexcept;
        ~Image();
    };
}