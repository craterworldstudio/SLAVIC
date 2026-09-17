#include "Brain.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace dhm::brain {

static std::vector<char> read_shader_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[BrainBootstrap] Failed to open shader: " + filename);
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);
    file.seekg(0);
    file.read(buffer.data(), file_size);
    return buffer;
}

static VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& code) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &module) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to create compute shader module.");
    }
    return module;
}

// Stored pipeline state handles (managed by BrainState or Brain)
struct ComputePipelineObjects {
    VkDescriptorSetLayout desc_set_layout{VK_NULL_HANDLE};
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    VkPipeline pipeline{VK_NULL_HANDLE};
    VkDescriptorPool descriptor_pool{VK_NULL_HANDLE};
    VkDescriptorSet descriptor_set{VK_NULL_HANDLE};
};

static ComputePipelineObjects g_pipeline_objects;

void initialize_compute_pipeline(GpuBrainContext& context, BrainState& state) {
    VkDevice device = context.get_device();
    std::cout << "[BrainBootstrap] Building Spiking Compute Pipeline..." << std::endl;

    // 1. Descriptor Set Layout (Binding 0 -> Neuron Storage Buffer)
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &layout_binding;

    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &g_pipeline_objects.desc_set_layout) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to create descriptor set layout.");
    }

    // 2. Push Constant Range
    VkPushConstantRange push_range{};
    push_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    push_range.offset = 0;
    push_range.size = sizeof(SimulationPushConstants);

    // 3. Pipeline Layout
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &g_pipeline_objects.desc_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_range;

    if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &g_pipeline_objects.pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to create pipeline layout.");
    }

    // 4. Load SPIR-V Shader
    auto shader_code = read_shader_file("src/Brain/Shaders/spv/neuron_step.spv");
    VkShaderModule comp_module = create_shader_module(device, shader_code);

    VkPipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_info.module = comp_module;
    stage_info.pName = "main";

    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage = stage_info;
    pipeline_info.layout = g_pipeline_objects.pipeline_layout;

    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &g_pipeline_objects.pipeline) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to create compute pipeline.");
    }

    vkDestroyShaderModule(device, comp_module, nullptr);

    // 5. Allocate Descriptor Pool & Set
    VkDescriptorPoolSize pool_size{};
    pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_size.descriptorCount = 1;

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = 1;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &g_pipeline_objects.descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to create descriptor pool.");
    }

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = g_pipeline_objects.descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &g_pipeline_objects.desc_set_layout;

    if (vkAllocateDescriptorSets(device, &alloc_info, &g_pipeline_objects.descriptor_set) != VK_SUCCESS) {
        throw std::runtime_error("[BrainBootstrap] Failed to allocate descriptor set.");
    }

    // 6. Bind Neuron Storage Buffer to Descriptor Set
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = state.get_neuron_buffer().buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(GpuNeuron) * state.get_neuron_count();

    VkWriteDescriptorSet write_desc{};
    write_desc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_desc.dstSet = g_pipeline_objects.descriptor_set;
    write_desc.dstBinding = 0;
    write_desc.descriptorCount = 1;
    write_desc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write_desc.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(device, 1, &write_desc, 0, nullptr);
    std::cout << "[BrainBootstrap] Spiking compute pipeline initialized and bound to VRAM." << std::endl;
}

// Expose pipeline objects for BrainClock
ComputePipelineObjects& get_active_pipeline() noexcept {
    return g_pipeline_objects;
}

} // namespace dhm::brain
