//episodic_memory.hpp
#pragma once
#include <vector>
#include <chrono>
#include "../types.hpp"
#include "experience.hpp"

struct MemoryEpisode {
    ActionType action;
    float outcome;
    float tension;
    float emotional_intensity;
    float strength;
    std::chrono::steady_clock::time_point timestamp;
};

class MemorySystem {
public:
    void store(ActionType action,
               float outcome,
               float tension,
               float emotional_intensity);

    void decay();
    void store_experience(const Experience& exp);
    float get_action_bias(ActionType action) const;
    float get_emotional_shift() const;

private:
    std::vector<MemoryEpisode> memories;
    const size_t MAX_MEMORIES = 1000;
};