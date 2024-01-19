//
// Created by gomkyung2 on 1/5/24.
//

#include <vkutil/Buffer.hpp>

#include <tuple>

vkutil::Buffer::Buffer(
    vma::Allocator allocator,
    const vk::BufferCreateInfo &createInfo,
    const vma::AllocationCreateInfo &allocInfo)
        : allocator { allocator } {
    std::tie(buffer, allocation) = allocator.createBuffer(createInfo, allocInfo);
}

vkutil::Buffer::Buffer(Buffer &&src) noexcept
    : allocator { src.allocator },
      buffer { std::exchange(src.buffer, nullptr) },
      allocation { std::exchange(src.allocation, nullptr) } {

}

vkutil::Buffer & vkutil::Buffer::operator=(Buffer &&src) noexcept {
    if (buffer && allocation) {
        allocator.destroyBuffer(buffer, allocation);
    }

    allocator = src.allocator;
    buffer = std::exchange(src.buffer, nullptr);
    allocation = std::exchange(src.allocation, nullptr);
    return *this;
}

vkutil::Buffer::~Buffer() {
    if (buffer && allocation) {
        allocator.destroyBuffer(buffer, allocation);
    }
}