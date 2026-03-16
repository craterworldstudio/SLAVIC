//episodic_memory.cpp

#include "episodic_memory.hpp"
#include <algorithm>
#include <cmath>

void MemorySystem::store(ActionType action,
                         float outcome,
                         float tension,
                         float emotional_intensity)
{
    float importance = std::abs(outcome) * emotional_intensity;

    if (importance < 0.05f)
        return; // ignore weak experiences

    MemoryEpisode ep;
    ep.action = action;
    ep.outcome = outcome;
    ep.tension = tension;
    ep.emotional_intensity = emotional_intensity;
    ep.strength = importance;
    ep.timestamp = std::chrono::steady_clock::now();

    memories.push_back(ep);

    if (memories.size() > MAX_MEMORIES) {
        auto weakest = std::min_element(memories.begin(), memories.end(),
            [](const MemoryEpisode& a, const MemoryEpisode& b) {
                return a.strength < b.strength;
            });
        memories.erase(weakest);
    }
}

void MemorySystem::decay()
{
    auto now = std::chrono::steady_clock::now();

    for (auto& m : memories) {
        float seconds =
            std::chrono::duration<float>(now - m.timestamp).count();

        m.strength *= std::exp(-0.1f * seconds);
    }

    memories.erase(
        std::remove_if(memories.begin(), memories.end(),
            [](const MemoryEpisode& m) {
                return m.strength < 0.01f;
            }),
        memories.end()
    );
}

void MemorySystem::store_experience(const Experience& exp)
{
    store(
        exp.action,
        exp.outcome,
        exp.tension,
        exp.emotional_intensity
    );
}

float MemorySystem::get_action_bias(ActionType action) const
{
    float bias = 0.0f;

    for (const auto& m : memories) {
        if (m.action == action)
            bias += m.outcome * m.strength;
    }

    return bias;
}

float MemorySystem::get_emotional_shift() const
{
    if (memories.empty()) return 0.0f;

    float total = 0.0f;
    
    for (const auto& m : memories) total += m.outcome * m.strength; 
    
    float average = total / memories.size(); 
    
    return average * 0.05f; // very small drift
}