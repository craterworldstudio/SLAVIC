//mind.hpp
#pragma once


#include <chrono>
#include "cognition/action_selector.hpp"
#include "drives/drive_system.hpp"
#include "../simulation/world_state.hpp"
#include <memory>
#include "emotion/emotion_system.hpp"
#include "memory/episodic_memory.hpp"

class Mind {
public:
    Mind();

    void tick();
    bool is_running() const;

    void load_state();
    void save_state();

private:
    DriveSystem drives;
    WorldState world;
    EmotionSystem emotion;
    ActionSelector action_selector;
    ActionType current_action; 
    MemorySystem memory;

    int tick_count;
    bool running;
    float last_outcome = 0.0f;
    float last_tension = 0.0f;
    std::chrono::steady_clock::time_point last_time;
    int tick_counter = 0;

    void perceive();
    void update_drives();
    void generate_thoughts();
    void select_action();
    void apply_action();
    void update_emotion();
    void store_experience();
    void adjust_identity();
};