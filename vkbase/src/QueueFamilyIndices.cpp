#include <vkbase/QueueFamilyIndices.hpp>

#include <optional>

vkbase::QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice physicalDevice) {
    std::optional<std::uint32_t> graphics, compute;
    for (std::uint32_t index = 0; vk::QueueFamilyProperties property : physicalDevice.getQueueFamilyProperties()) {
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics = index;
        }
        if (property.queueFlags & vk::QueueFlagBits::eCompute) {
            compute = index;
        }

        if (graphics && compute) {
            this->graphics = *graphics;
            this->compute = *compute;
            return;
        }

        ++index;
    }

    throw std::runtime_error { "Failed to get required queue family indices from physical device" };
}

std::vector<std::uint32_t> vkbase::QueueFamilyIndices::getUniqueIndices() const noexcept {
    if (graphics == compute) {
        return { graphics };
    }

    return { graphics, compute };
}
