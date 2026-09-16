#include "BrainState.h"
#include <iostream>

namespace dhm::brain {

BrainState::BrainState(GpuBrainContext& context)
    : context_(context) {}

BrainState::~BrainState() {
    if (neuron_buffer_.buffer != VK_NULL_HANDLE) {
        context_.destroy_buffer(neuron_buffer_);
    }
}

void BrainState::allocate(uint32_t neuron_count) {
    if (neuron_buffer_.buffer != VK_NULL_HANDLE) {
        context_.destroy_buffer(neuron_buffer_);
    }

    neuron_count_ = neuron_count;
    push_constants_.total_neurons = neuron_count;

    const VkDeviceSize buffer_size = sizeof(GpuNeuron) * neuron_count;
    
    // Usage: Storage buffer for compute shader, Transfer dst for staging, Vertex buffer for visualizer
    const VkBufferUsageFlags usage = 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // Fast Device-Local VRAM allocation
    const VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    neuron_buffer_ = context_.create_buffer(buffer_size, usage, properties);

    std::cout << "[BrainState] Allocated " << neuron_count 
              << " neurons in VRAM (" << (buffer_size / (1024 * 1024)) << " MB)" << std::endl;
}

} // namespace dhm::brain
