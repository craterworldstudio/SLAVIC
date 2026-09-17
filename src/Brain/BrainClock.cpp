#include "Brain.h"
#include <iostream>

namespace dhm::brain {

struct ComputePipelineObjects {
    VkDescriptorSetLayout desc_set_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_set;
};

ComputePipelineObjects& get_active_pipeline() noexcept;

void dispatch_simulation_step(GpuBrainContext& context, BrainState& state, float delta_time_ms) {
    VkDevice device = context.get_device();
    VkQueue compute_queue = context.get_compute_queue();
    VkCommandPool command_pool = context.get_command_pool();
    auto& pipeline = get_active_pipeline();

    // Allocate transient command buffer
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer cmd_buffer;
    if (vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer) != VK_SUCCESS) {
        return;
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    // Bind Spiking Compute Pipeline & Descriptor
    vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline);
    vkCmdBindDescriptorSets(
        cmd_buffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        pipeline.pipeline_layout,
        0, 1,
        &pipeline.descriptor_set,
        0, nullptr
    );

    // Push Updated Timestep & Neuromodulator Bath (Dopamine / Noradrenaline)
    auto push_constants = state.get_constants();
    push_constants.dt = delta_time_ms;

    vkCmdPushConstants(
        cmd_buffer,
        pipeline.pipeline_layout,
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        sizeof(SimulationPushConstants),
        &push_constants
    );

    // Dispatch: Workgroup size is 64 threads
    uint32_t workgroups = (state.get_neuron_count() + 63) / 64;
    vkCmdDispatch(cmd_buffer, workgroups, 1, 1);

    vkEndCommandBuffer(cmd_buffer);

    // Submit Compute Batch
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;

    vkQueueSubmit(compute_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(compute_queue);

    vkFreeCommandBuffers(device, command_pool, 1, &cmd_buffer);
}

} // namespace dhm::brain
