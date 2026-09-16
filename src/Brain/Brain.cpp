#include "Brain.h"
#include <iostream>

namespace dhm::brain {

Brain::Brain() {
    context_.initialize();
    state_ = std::make_unique<BrainState>(context_);
}

Brain::~Brain() {
    shutdown();
}

void Brain::bootstrap(const genome::Genome& genome) {
    std::cout << "[Brain] Reading genome: " << genome.get_lineage_name() << std::endl;

    // Read developmental parameters from digital genome
    auto hormone_params = genome.express_subsystem(
        genome::SubsystemType::Hormones, 
        genome::DevelopmentalStage::Embryo
    );

    float dopamine_baseline = 0.05f;
    float noradrenaline_gain = 0.80f;

    if (hormone_params.contains("dopamine_clearance_rate")) {
        dopamine_baseline = static_cast<float>(hormone_params["dopamine_clearance_rate"]);
    }
    if (hormone_params.contains("noradrenaline_arousal_gain")) {
        noradrenaline_gain = static_cast<float>(hormone_params["noradrenaline_arousal_gain"]);
    }

    state_->set_modulators(dopamine_baseline, noradrenaline_gain);

    // Initial embryonic allocation: 100,000 protoneurons
    constexpr uint32_t INITIAL_EMBRYO_NEURONS = 100'000;
    state_->allocate(INITIAL_EMBRYO_NEURONS);

    is_running_ = true;
    std::cout << "[Brain] Embryonic bootstrap complete. Online." << std::endl;
}

void Brain::tick(float delta_time_ms) {
    if (!is_running_) return;

    current_tick_++;
    // Future step: vkCmdDispatch compute shader pass for neuron dynamics
}

void Brain::inject_neuromodulators(float dopamine, float noradrenaline) noexcept {
    if (state_) {
        state_->set_modulators(dopamine, noradrenaline);
    }
}

void Brain::shutdown() noexcept {
    if (is_running_) {
        std::cout << "[Brain] Shutting down simulation clock..." << std::endl;
        is_running_ = false;
    }
}

} // namespace dhm::brain
