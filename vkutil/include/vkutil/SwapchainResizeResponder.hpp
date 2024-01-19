//
// Created by gomkyung2 on 1/13/24.
//

#pragma once

import vulkan_hpp;

namespace vkutil{
    // TODO.CXX23: redundant CRTP (using explicit object parameter)
    template <typename Self>
    struct SwapchainResizeResponder {
        void onSwapchainResized(vk::Extent2D extent) {
            return static_cast<Self *>(this)->onSwapchainResized(extent);
        }
    };
}