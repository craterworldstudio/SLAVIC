#include "GpuBrainContext.h"
#include <iostream>
#include <cstring>

namespace dhm::brain {

GpuBrainContext::GpuBrainContext() = default;

GpuBrainContext::~GpuBrainContext() {
    cleanup();
}

GpuBrainContext::GpuBrainContext(GpuBrainContext&& other) noexcept {
    *this = std::move(other);
}

GpuBrainContext& GpuBrainContext::operator=(GpuBrainContext&& other) noexcept {
    if (this != &other) {
        cleanup();
        instance_ = other.instance_;
        physical_device_ = other.physical_device_;
        device_ = other.device_;
        compute_queue_ = other.compute_queue_;
        transfer_queue_ = other.transfer_queue_;
        queue_indices_ = other.queue_indices_;
        command_pool_ = other.command_pool_;

        other.instance_ = VK_NULL_HANDLE;
        other.physical_device_ = VK_NULL_HANDLE;
        other.device_ = VK_NULL_HANDLE;
        other.command_pool_ = VK_NULL_HANDLE;
    }
    return *this;
}

void GpuBrainContext::initialize() {
    create_instance();
    pick_physical_device();
    create_logical_device();
    create_command_pool();
    std::cout << "[GpuBrainContext] Vulkan Compute initialized successfully." << std::endl;
}

void GpuBrainContext::create_instance() {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "DHM-Core Spiking Brain";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.pEngineName = "DHM-ComputeEngine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
        throw std::runtime_error("[GpuBrainContext] Failed to create Vulkan instance.");
    }
}

QueueFamilyIndices GpuBrainContext::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        // Look for dedicated compute queue family
        if (families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.compute_family = i;
        }
        if (families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transfer_family = i;
        }
        if (indices.is_complete()) break;
    }
    return indices;
}

void GpuBrainContext::pick_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("[GpuBrainContext] No Vulkan-compatible GPUs found.");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

    for (const auto& dev : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);

        // Prioritize Discrete GPUs
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            auto indices = find_queue_families(dev);
            if (indices.is_complete()) {
                physical_device_ = dev;
                queue_indices_ = indices;
                std::cout << "[GpuBrainContext] Bound Device: " << props.deviceName << std::endl;
                break;
            }
        }
    }

    if (physical_device_ == VK_NULL_HANDLE) {
        throw std::runtime_error("[GpuBrainContext] Suitable discrete GPU not found.");
    }
}

void GpuBrainContext::create_logical_device() {
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    float priority = 1.0f;

    VkDeviceQueueCreateInfo compute_info{};
    compute_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    compute_info.queueFamilyIndex = queue_indices_.compute_family.value();
    compute_info.queueCount = 1;
    compute_info.pQueuePriorities = &priority;
    queue_create_infos.push_back(compute_info);

    VkPhysicalDeviceFeatures device_features{};
    device_features.shaderFloat64 = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;

    if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("[GpuBrainContext] Failed to create logical device.");
    }

    vkGetDeviceQueue(device_, queue_indices_.compute_family.value(), 0, &compute_queue_);
}

void GpuBrainContext::create_command_pool() {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_indices_.compute_family.value();

    if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
        throw std::runtime_error("[GpuBrainContext] Failed to create compute command pool.");
    }
}

uint32_t GpuBrainContext::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && 
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("[GpuBrainContext] Failed to find matching memory type.");
}

GpuBuffer GpuBrainContext::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    GpuBuffer gpu_buffer{};
    gpu_buffer.size = size;

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_, &buffer_info, nullptr, &gpu_buffer.buffer) != VK_SUCCESS) {
        throw std::runtime_error("[GpuBrainContext] Failed to create VkBuffer.");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device_, &gpu_buffer.buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_, &alloc_info, nullptr, &gpu_buffer.memory) != VK_SUCCESS) {
        throw std::runtime_error("[GpuBrainContext] Failed to allocate device memory for buffer.");
    }

    vkBindBufferMemory(device_, gpu_buffer.buffer, gpu_buffer.memory, 0);
    return gpu_buffer;
}

void GpuBrainContext::destroy_buffer(GpuBuffer& buffer) noexcept {
    if (buffer.mapped_ptr && device_) {
        vkUnmapMemory(device_, buffer.memory);
        buffer.mapped_ptr = nullptr;
    }
    if (buffer.buffer != VK_NULL_HANDLE && device_) {
        vkDestroyBuffer(device_, buffer.buffer, nullptr);
        buffer.buffer = VK_NULL_HANDLE;
    }
    if (buffer.memory != VK_NULL_HANDLE && device_) {
        vkFreeMemory(device_, buffer.memory, nullptr);
        buffer.memory = VK_NULL_HANDLE;
    }
    buffer.size = 0;
}

void GpuBrainContext::cleanup() noexcept {
    if (device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_);
        if (command_pool_ != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device_, command_pool_, nullptr);
            command_pool_ = VK_NULL_HANDLE;
        }
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }
    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }
}

} // namespace dhm::brain
