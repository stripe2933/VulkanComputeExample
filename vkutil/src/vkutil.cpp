//
// Created by gomkyung2 on 1/5/24.
//

#include <vkutil/vkutil.hpp>

#include <fstream>

std::vector<std::byte> readFileAsBytes(const std::filesystem::path &path) {
    std::ifstream file { path, std::ios::ate | std::ios::binary };
    if (!file.is_open()) {
        throw std::runtime_error { "Failed to open file" };
    }

    const std::size_t fileSize = file.tellg();
    std::vector<std::byte> buffer(fileSize);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    return buffer;
}

vk::ShaderModule vkutil::createShaderModule(vk::Device device, std::span<const std::byte> source) {
    const vk::ShaderModuleCreateInfo createInfo {
        {}, source.size(), reinterpret_cast<const std::uint32_t*>(source.data())
    };
    return device.createShaderModule(createInfo);
}

vk::ShaderModule vkutil::createShaderModule(vk::Device device, const std::filesystem::path &path) {
    const std::vector code = readFileAsBytes(path);
    return createShaderModule(device, code);
}

vk::raii::ShaderModule vkutil::createShaderModule(const vk::raii::Device &device, std::span<const std::byte> source) {
    return { device, createShaderModule(*device, source) };
}

vk::raii::ShaderModule vkutil::createShaderModule(const vk::raii::Device &device, const std::filesystem::path &path) {
    return { device, createShaderModule(*device, path) };
}

vk::ImageView vkutil::createImageView(vk::Device device, vk::Image image, vk::ImageAspectFlags aspect, vk::Format format) {
    const vk::ImageViewCreateInfo createInfo {
        {},
        image,
        vk::ImageViewType::e2D,
        format,
        {},
        { aspect, 0, 1, 0, 1 }
    };
    return device.createImageView(createInfo);
}

vk::raii::ImageView vkutil::createImageView(const vk::raii::Device &device, vk::Image image, vk::ImageAspectFlags aspect, vk::Format format) {
    return { device, createImageView(*device, image, aspect, format) };
}
