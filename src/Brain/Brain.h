#ifndef DHM_BRAIN_BRAIN_H
#define DHM_BRAIN_BRAIN_H

#include "GpuBrainContext.h"
#include "BrainState.h"
#include "../Genome/Genome.h"
#include <memory>
#include <cstdint>

namespace dhm::brain {

class Brain {
public:
    Brain();
    ~Brain();

    // Lifecycle
    void bootstrap(const genome::Genome& genome);
    void tick(float delta_time_ms);
    void shutdown() noexcept;

    // Subsystem Modulators
    void inject_neuromodulators(float dopamine, float noradrenaline) noexcept;

    // Getters
    [[nodiscard]] BrainState& get_state() noexcept { return *state_; }
    [[nodiscard]] const BrainState& get_state() const noexcept { return *state_; }
    [[nodiscard]] GpuBrainContext& get_context() noexcept { return context_; }

private:
    GpuBrainContext context_;
    std::unique_ptr<BrainState> state_;
    uint64_t current_tick_{0};
    bool is_running_{false};
};

} // namespace dhm::brain

#endif // DHM_BRAIN_BRAIN_H
