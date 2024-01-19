//
// Created by gomkyung2 on 1/4/24.
//

#include <vkbase/Queues.hpp>

vkbase::Queues::Queues(vk::Device device, QueueFamilyIndices queueFamilyIndices)
    : graphics { device.getQueue(queueFamilyIndices.graphics, 0) },
      compute { device.getQueue(queueFamilyIndices.compute, 0) } {
}
