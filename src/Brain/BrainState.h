#ifndef DHM_BRAIN_BRAIN_STATE_H
#define DHM_BRAIN_BRAIN_STATE_H

#include "GpuBrainContext.h"
#include <cstdint>

namespace dhm::brain {

// Packed representation matching the GPU compute shader memory layout
struct alignas(16) GpuNeuron {
    float x, y, z;               // 3D Spatial position
    float v;                     // Membrane potential (mV)
    float u;                     // Membrane recovery variable
    uint32_t fired;              // 1 if active spike, 0 otherwise
    uint32_t layer_type;         // 0: Sensory, 1: Subcortex, 2: Cortex, 3: Motor
    uint32_t refractory_ticks;   // Refractory period counter
};

// Push constant payload passed to shader each simulation step
struct alignas(16) SimulationPushConstants {
    float dopamine_level;        // Global neuromodulator scaling
    float noradrenaline_level;   // Global gain / threshold modulation
    float dt;                    // Delta time step (e.g., 0.5 ms)
    uint32_t total_neurons;
};

class BrainState {
public:
    explicit BrainState(GpuBrainContext& context);
    ~BrainState();

    void allocate(uint32_t neuron_count);
    
    [[nodiscard]] const GpuBuffer& get_neuron_buffer() const noexcept { return neuron_buffer_; }
    [[nodiscard]] uint32_t get_neuron_count() const noexcept { return neuron_count_; }

    void set_modulators(float dopamine, float noradrenaline) noexcept {
        push_constants_.dopamine_level = dopamine;
        push_constants_.noradrenaline_level = noradrenaline;
    }

    [[nodiscard]] const SimulationPushConstants& get_constants() const noexcept {
        return push_constants_;
    }

private:
    GpuBrainContext& context_;
    uint32_t neuron_count_{0};
    GpuBuffer neuron_buffer_{};
    SimulationPushConstants push_constants_{0.0f, 0.0f, 0.5f, 0};
};

} // namespace dhm::brain

#endif // DHM_BRAIN_BRAIN_STATE_H
