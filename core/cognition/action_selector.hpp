//action_selector.hpp
#pragma once
#include "../drives/drive_system.hpp"
#include "../emotion/emotion_system.hpp"
#include "../memory/episodic_memory.hpp"
#include "../types.hpp"

class ActionSelector {
public:
    ActionType select(const DriveSystem& drives, const EmotionState& emotion, const MemorySystem& memory);

private:
    float normalize_and_pick(float e, float r, float s, float st);
};