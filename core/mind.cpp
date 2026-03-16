//mind.cpp
#include "mind.hpp"
#include <thread>
#include <iostream>

Mind::Mind() : tick_count(0), running(true), current_action(ActionType::Stabilize) {
    last_time = std::chrono::steady_clock::now();
}

bool Mind::is_running() const {
    return running;
}

void Mind::load_state() {
    std::cout << "Loading state...\n";
}

void Mind::save_state() {
    std::cout << "Saving state...\n";
}

void Mind::tick() {
    tick_count++;

    perceive();
    update_drives();
    generate_thoughts();
    select_action();
    apply_action();
    update_emotion();
    if (tick_count % 500 == 0) memory.decay();
    store_experience();
    adjust_identity();

    //std::cout << "[Tick " << tick_count << "] complete.\n";

    if (tick_count > 5'000'000)  // temporary limiter 
        running = false;
    //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    tick_counter++;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_time);

    if (elapsed.count() >= 1) {
        std::cout << "\r\033[K"
                  << "TPS: " << tick_counter
                  << " | Tick: " << tick_count
                  << " | Action: " << (int)current_action
                  << " | Tension: " << last_tension
                  << " | Fragility: " << emotion.fragility
                  << std::flush;

        tick_counter = 0;
        last_time = now;
    }
}




void Mind::perceive() {
    world.update();
}

void Mind::update_drives() {
    drives.update(world.threat, world.resource, world.social_presence);

    last_tension = drives.get_internal_tension();
    //std::cout << "Action: " << (int)current_action << "\n";
    //std::cout << "Tension: " << last_tension << "\n" << "Fragility: " << emotion.fragility << "\n";
}

void Mind::generate_thoughts() {}

void Mind::select_action() {
    current_action = action_selector.select(
        drives,
        emotion.get_state(),
        memory
    );
}

void Mind::apply_action() {

    switch (current_action) {

        case ActionType::Explore:
            world.resource += 0.05f;
            world.threat += 0.02f;

            last_outcome = (rand() % 100 < 60) ? 0.3f : -0.3f;
            break;

        case ActionType::Retreat:
            world.threat -= 0.05f;
            world.resource -= 0.02f;

            last_outcome = 0.1f;  // safe but low reward
            break;

        case ActionType::Socialize:
            world.social_presence += 0.05f;
            world.threat += 0.01f;

            last_outcome = (rand() % 100 < 50) ? 0.2f : -0.2f;
            break;

        case ActionType::Stabilize:
            drives.reduce_tension(0.05f);
            world.resource -= 0.01f;
            
            last_outcome = 0.15f;
            break;
    }
}
void Mind::update_emotion() {
    emotion.update(last_tension, last_outcome);
    emotion.decay();
    emotion.apply_baseline_shift(memory.get_emotional_shift());
}

void Mind::store_experience()
{
    float novelty = world.get_novelty();   // or temporary constant
    float emotional_intensity = emotion.get_intensity();

    Experience exp(
        current_action,
        last_outcome,
        last_tension,
        novelty,
        emotional_intensity
    );

    memory.store_experience(exp);
}

void Mind::adjust_identity() {}