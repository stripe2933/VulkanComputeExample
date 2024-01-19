//
// Created by gomkyung2 on 1/5/24.
//

#include <vkbase/AppWithSwapchain.hpp>

void vkbase::AppWithSwapchain::recreateSwapchain(vk::Format format, vk::ColorSpaceKHR colorSpace, vk::PresentModeKHR presentMode, vk::Extent2D extent) {
    const vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);

    const vk::SwapchainCreateInfoKHR createInfo {
        {},
        *surface,
        static_cast<uint32_t>(swapchainImages.size()),
        format,
        colorSpace,
        extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive,
        {},
        capabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        {},
        *swapchain.swapchain,
    };
    swapchain = { { device, createInfo }, format, colorSpace, extent };

    swapchainImages = swapchain.swapchain.getImages();

    swapchainImageViews.clear();
    swapchainImageViews.reserve(swapchainImages.size());
    std::ranges::transform(swapchainImages, back_inserter(swapchainImageViews), [this](vk::Image image) {
        const vk::ImageViewCreateInfo createInfo {
            {},
            image,
            vk::ImageViewType::e2D,
            swapchain.format,
            {},
            { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 },
        };
        return vk::raii::ImageView { device, createInfo };
    });
}
