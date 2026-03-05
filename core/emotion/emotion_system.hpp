#pragma once

struct EmotionState {
    float anxiety = 0.0f;
    float curiosity_bias = 0.0f;
    float fear_bias = 0.0f;
};

class EmotionSystem {
public:

    float fragility;
    EmotionSystem();

    void update(float tension, float outcome);
    void decay();
    void apply_baseline_shift(float shift);

    EmotionState get_state() const;

private:
          // heals over time
    float ego_strength;   // fluctuates
    EmotionState state;

    void clamp(float& v);
};