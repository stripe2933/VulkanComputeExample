//
// Created by gomkyung2 on 1/4/24.
//

#pragma once

#include <glm/glm.hpp>

import vulkan_hpp;

struct GLFWwindow;

namespace vkutil {
    class GlfwWindow {
        GLFWwindow *base;

    protected:
        virtual void onSizeCallback(glm::ivec2 size) { }
        virtual void onFramebufferSizeCallback(glm::ivec2 size) { }
        virtual void onContentScaleCallback(glm::vec2 scale) { }
        virtual void onKeyCallback(int key, int scancode, int action, int mods) { }
        virtual void onCharCallback(unsigned int codepoint) { }
        virtual void onCursorPosCallback(glm::dvec2 position) { }
        virtual void onCursorEnterCallback(int entered) { }
        virtual void onMouseButtonCallback(int button, int action, int mods) { }
        virtual void onScrollCallback(glm::dvec2 offset) { }
        virtual void onDropCallback(int count, const char ** paths) { }

        virtual void beforeLoop() { }
        virtual void onLoop(float timeDelta) { };
        virtual void afterLoop() { }

    public:
        GlfwWindow(int width, int height, const char *title);
        GlfwWindow(const GlfwWindow&) = delete;
        GlfwWindow(GlfwWindow&&) noexcept;
        GlfwWindow &operator=(const GlfwWindow&) = delete;
        GlfwWindow &operator=(GlfwWindow&&) noexcept;
        virtual ~GlfwWindow();

        [[nodiscard]] explicit operator GLFWwindow*() const;

        [[nodiscard]] vk::SurfaceKHR createSurface(vk::Instance instance) const;

        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] GLFWwindow *getPointer();
        [[nodiscard]] glm::ivec2 getSize() const;
        [[nodiscard]] glm::ivec2 getFramebufferSize() const;
        [[nodiscard]] float getFramebufferAspectRatio() const;
        [[nodiscard]] glm::dvec2 getCursorPos() const;
        [[nodiscard]] glm::dvec2 getFramebufferCursorPos() const;
        [[nodiscard]] glm::vec2 getContentScale() const;

        void run();
    };
}