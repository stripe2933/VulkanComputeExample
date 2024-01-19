//
// Created by gomkyung2 on 1/4/24.
//

#include <vkutil/GlfwWindow.hpp>

#include <stdexcept>
#include <utility>

using VkInstance = vk::Instance::CType;
using VkSurfaceKHR = vk::SurfaceKHR::CType;
using VkPhysicalDevice = vk::PhysicalDevice::CType;
using VkAllocationCallbacks = vk::AllocationCallbacks;
using VkResult = vk::Result;
#define VK_VERSION_1_0
#include <GLFW/glfw3.h>

vkutil::GlfwWindow::GlfwWindow(int width, int height, const char *title)
    : base { glfwCreateWindow(width, height, title, nullptr, nullptr) } {
    glfwSetWindowUserPointer(base, this);

    glfwSetWindowSizeCallback(base, [](GLFWwindow *window, int width, int height) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onSizeCallback({ width, height });
    });
    glfwSetFramebufferSizeCallback(base, [](GLFWwindow *window, int width, int height) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onFramebufferSizeCallback({ width, height });
    });
    glfwSetWindowContentScaleCallback(base, [](GLFWwindow *window, float xscale, float yscale) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onContentScaleCallback({ xscale, yscale });
    });
    glfwSetKeyCallback(base, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onKeyCallback(key, scancode, action, mods);
    });
    glfwSetCharCallback(base, [](GLFWwindow *window, unsigned int codepoint) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onCharCallback(codepoint);
    });
    glfwSetCursorPosCallback(base, [](GLFWwindow *window, double xpos, double ypos) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onCursorPosCallback({ xpos, ypos });
    });
    glfwSetCursorEnterCallback(base, [](GLFWwindow *window, int entered) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onCursorEnterCallback(entered);
    });
    glfwSetMouseButtonCallback(base, [](GLFWwindow *window, int button, int action, int mods) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onMouseButtonCallback(button, action, mods);
    });
    glfwSetScrollCallback(base, [](GLFWwindow *window, double xoffset, double yoffset) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onScrollCallback({ xoffset, yoffset });
    });
    glfwSetDropCallback(base, [](GLFWwindow *window, int count, const char **paths) {
        auto *base = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        base->onDropCallback(count, paths);
    });
}

vkutil::GlfwWindow::GlfwWindow(GlfwWindow &&src) noexcept
    : base { std::exchange(src.base, nullptr) } {
    glfwSetWindowUserPointer(base, this);
}

vkutil::GlfwWindow & vkutil::GlfwWindow::operator=(GlfwWindow &&src) noexcept {
    base = std::exchange(src.base, nullptr);
    glfwSetWindowUserPointer(base, this);
    return *this;
}

vkutil::GlfwWindow::~GlfwWindow() {
    if (base) {
        glfwDestroyWindow(base);
    }
}

vkutil::GlfwWindow::operator GLFWwindow*() const {
    return base;
}

vk::SurfaceKHR vkutil::GlfwWindow::createSurface(vk::Instance instance) const {
    vk::SurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, base, nullptr, &reinterpret_cast<vk::SurfaceKHR::CType&>(surface)) != vk::Result::eSuccess) {
        throw std::runtime_error { "Failed to create window surface" };
    }

    return surface;
}

bool vkutil::GlfwWindow::shouldClose() const {
    return glfwWindowShouldClose(base);
}

GLFWwindow * vkutil::GlfwWindow::getPointer() {
    return base;
}

glm::ivec2 vkutil::GlfwWindow::getSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(base, &size.x, &size.y);
    return size;
}

glm::ivec2 vkutil::GlfwWindow::getFramebufferSize() const {
    glm::ivec2 size;
    glfwGetFramebufferSize(base, &size.x, &size.y);
    return size;
}

float vkutil::GlfwWindow::getFramebufferAspectRatio() const {
    const glm::ivec2 size = getFramebufferSize();
    return static_cast<float>(size.x) / static_cast<float>(size.y);
}

glm::dvec2 vkutil::GlfwWindow::getCursorPos() const {
    glm::dvec2 pos;
    glfwGetCursorPos(base, &pos.x, &pos.y);
    return pos;
}

glm::dvec2 vkutil::GlfwWindow::getFramebufferCursorPos() const {
    return glm::dvec2 { getContentScale() } * getCursorPos();
}

glm::vec2 vkutil::GlfwWindow::getContentScale() const {
    glm::vec2 scale;
    glfwGetWindowContentScale(base, &scale.x, &scale.y);
    return scale;
}

void vkutil::GlfwWindow::run() {
    beforeLoop();

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(base)) {
        glfwPollEvents();

        const float timeDelta = glfwGetTime() - lastTime;
        lastTime += timeDelta;

        onLoop(timeDelta);
    }

    afterLoop();
}