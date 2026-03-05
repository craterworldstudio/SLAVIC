#include "emotion_system.hpp"
#include <algorithm>

EmotionSystem::EmotionSystem()
    : fragility(0.2f), ego_strength(0.6f) {}

void EmotionSystem::update(float tension, float outcome) {

    if (outcome < 0.0f) {
        fragility += 0.02f;
        ego_strength -= 0.01f;
    }

    // anxiety scales with tension + fragility
    state.anxiety = tension * (1.0f + fragility);

    state.fear_bias = state.anxiety * 0.5f;
    state.curiosity_bias = -state.anxiety * 0.3f;

    clamp(fragility);
    clamp(ego_strength);
}

void EmotionSystem::decay() {
    fragility *= 0.995f;   // healing (Option 1)
}

EmotionState EmotionSystem::get_state() const {
    return state;
}

void EmotionSystem::clamp(float& v) {
    v = std::clamp(v, 0.0f, 1.0f);
}

void EmotionSystem::apply_baseline_shift(float shift) {
    fragility += shift;
    clamp(fragility);
}