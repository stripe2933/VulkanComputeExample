//
// Created by gomkyung2 on 1/5/24.
//

module;

#include <vkutil/Buffer.hpp>
#include <vkutil/BufferStager.hpp>
#include <vkutil/CommandRecorder.hpp>
#include <vkutil/Image.hpp>
#include <vkutil/ImageStager.hpp>
#include <vkutil/PersistentMappedBuffer.hpp>
#include <vkutil/SwapchainResizeResponder.hpp>
#include <vkutil/vkutil.hpp>

#if VKUTIL_USE_GLFW_WINDOW
#include <vkutil/GlfwWindow.hpp>
#endif

export module vkutil;
export import vk_mem_alloc_hpp;

export namespace vkutil{
    using vkutil::Buffer;
    using vkutil::BufferStager;
    using vkutil::CommandRecorder;
#if VKUTIL_USE_GLFW_WINDOW
    using vkutil::GlfwWindow;
#endif
    using vkutil::Image;
    using vkutil::ImageStager;
    using vkutil::PersistentMappedBuffer;
    using vkutil::SwapchainResizeResponder;

    using vkutil::createShaderModule;
    using vkutil::createImageView;
    using vkutil::executeSingleCommand;
}