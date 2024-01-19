//
// Created by gomkyung2 on 1/13/24.
//

#include <vkutil/PersistentMappedBuffer.hpp>

#include <utility>

vkutil::PersistentMappedBuffer::PersistentMappedBuffer(vma::Allocator allocator, const vk::BufferCreateInfo &createInfo)
    : Buffer { allocator, createInfo, allocInfo } {
    data = static_cast<std::byte *>(allocator.mapMemory(allocation));
}

vkutil::PersistentMappedBuffer::PersistentMappedBuffer(PersistentMappedBuffer &&src) noexcept
    : Buffer { std::move(src) },
      data { src.data }{
}

vkutil::PersistentMappedBuffer & vkutil::PersistentMappedBuffer::operator=(PersistentMappedBuffer &&src) noexcept {
    Buffer::operator=(std::move(src));
    data = src.data;
    return *this;
}

vkutil::PersistentMappedBuffer::~PersistentMappedBuffer() {
    if (buffer && allocation) {
        allocator.unmapMemory(allocation);
    }
}
