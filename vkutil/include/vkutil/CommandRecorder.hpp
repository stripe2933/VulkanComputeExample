//
// Created by gomkyung2 on 1/5/24.
//

#pragma once

#include <utility>

import vulkan_hpp;

#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

namespace vkutil{
    // TODO.CXX23: redundant CRTP (using explicit object parameter)
    template <typename Self, typename Result = void, typename... Args>
    struct CommandRecorder {
        using result_type = Result;
        template <std::size_t I>
        using arg_type = std::tuple_element_t<I, std::tuple<Args...>>;

        [[nodiscard]] Result recordCommand(vk::CommandBuffer commandBuffer, auto &&...args) const {
            return static_cast<const Self *>(this)->recordCommand(commandBuffer, FWD(args)...);
        }
    };
}