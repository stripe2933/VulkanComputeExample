//
// Created by gomkyung2 on 1/5/24.
//

#include <vkutil/Image.hpp>

#include <tuple>

vkutil::Image::Image(
    vma::Allocator allocator,
    const vk::ImageCreateInfo &createInfo,
    const vma::AllocationCreateInfo &allocInfo)
        : allocator { allocator } {
    std::tie(image, allocation) = allocator.createImage(createInfo, allocInfo);
}

vkutil::Image::Image(Image &&src) noexcept
    : allocator { src.allocator },
      image { std::exchange(src.image, nullptr) },
      allocation { std::exchange(src.allocation, nullptr) } {
}

vkutil::Image & vkutil::Image::operator=(Image &&src) noexcept {
    if (image && allocation) {
        allocator.destroyImage(image, allocation);
    }

    allocator = src.allocator;
    image = std::exchange(src.image, nullptr);
    allocation = std::exchange(src.allocation, nullptr);
    return *this;
}

vkutil::Image::~Image() {
    if (image && allocation) {
        allocator.destroyImage(image, allocation);
    }
}