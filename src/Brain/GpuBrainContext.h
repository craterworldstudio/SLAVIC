#ifndef DHM_BRAIN_GPU_BRAIN_CONTEXT_H
#define DHM_BRAIN_GPU_BRAIN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <stdexcept>

namespace dhm::brain {

struct QueueFamilyIndices {
    std::optional<uint32_t> compute_family;
    std::optional<uint32_t> transfer_family;

    [[nodiscard]] constexpr bool is_complete() const noexcept {
        return compute_family.has_value() && transfer_family.has_value();
    }
};

struct GpuBuffer {
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkDeviceSize size{0};
    void* mapped_ptr{nullptr};
};

class GpuBrainContext {
public:
    GpuBrainContext();
    ~GpuBrainContext();

    GpuBrainContext(const GpuBrainContext&) = delete;
    GpuBrainContext& operator=(const GpuBrainContext&) = delete;
    GpuBrainContext(GpuBrainContext&&) noexcept;
    GpuBrainContext& operator=(GpuBrainContext&&) noexcept;

    void initialize();
    void cleanup() noexcept;

    // Buffer Allocation Helpers
    [[nodiscard]] GpuBuffer create_buffer(
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties
    );
    void destroy_buffer(GpuBuffer& buffer) noexcept;

    // Direct Getters
    [[nodiscard]] VkDevice get_device() const noexcept { return device_; }
    [[nodiscard]] VkPhysicalDevice get_physical_device() const noexcept { return physical_device_; }
    [[nodiscard]] VkQueue get_compute_queue() const noexcept { return compute_queue_; }
    [[nodiscard]] uint32_t get_compute_queue_family() const noexcept { return queue_indices_.compute_family.value(); }
    [[nodiscard]] VkCommandPool get_command_pool() const noexcept { return command_pool_; }

private:
    VkInstance instance_{VK_NULL_HANDLE};
    VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
    VkDevice device_{VK_NULL_HANDLE};
    
    VkQueue compute_queue_{VK_NULL_HANDLE};
    VkQueue transfer_queue_{VK_NULL_HANDLE};
    QueueFamilyIndices queue_indices_;

    VkCommandPool command_pool_{VK_NULL_HANDLE};

    void create_instance();
    void pick_physical_device();
    void create_logical_device();
    void create_command_pool();
    
    [[nodiscard]] uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    [[nodiscard]] QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
};

} // namespace dhm::brain

#endif // DHM_BRAIN_GPU_BRAIN_CONTEXT_H
