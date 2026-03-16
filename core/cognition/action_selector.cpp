//action_selector.cpp

#include "action_selector.hpp"
#include <cstdlib>

ActionType ActionSelector::select(
    const DriveSystem& drives,
    const EmotionState& emotion,
    const MemorySystem& memory
) {

    float explore_score =
        drives.get_drive("curiosity") * (1.0f + emotion.curiosity_bias);

    explore_score += memory.get_action_bias(ActionType::Explore);

    float retreat_score =
        drives.get_drive("stability") * (1.0f + emotion.fear_bias);

    retreat_score += memory.get_action_bias(ActionType::Retreat);

    float social_score =
        drives.get_drive("social");

    social_score += memory.get_action_bias(ActionType::Socialize);

    float stabilize_score =
        drives.get_drive("coherence");
        
    stabilize_score += memory.get_action_bias(ActionType::Stabilize);

    explore_score = std::max(0.01f, explore_score);
    retreat_score = std::max(0.01f, retreat_score);
    social_score = std::max(0.01f, social_score);
    stabilize_score = std::max(0.01f, stabilize_score);
    
    
    float total = explore_score + retreat_score +
                  social_score + stabilize_score;

    float roll = ((float)rand() / RAND_MAX) * total;

    if (roll < explore_score)
        return ActionType::Explore;
    else if (roll < explore_score + retreat_score)
        return ActionType::Retreat;
    else if (roll < explore_score + retreat_score + social_score)
        return ActionType::Socialize;
    else
        return ActionType::Stabilize;
}