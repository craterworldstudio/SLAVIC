#include "mind.hpp"
#include "../SLAVIC/simulation/world_state.hpp"
#include <iostream>

WorldState world;
Mind::Mind() : tick_count(0), running(true) {}

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
    store_experience();
    adjust_identity();

    std::cout << "[Tick " << tick_count << "] complete.\n";

    if (tick_count > 50)  // temporary limiter 
        running = false;
}

void Mind::perceive() {
    world.update();
}

void Mind::update_drives() {}
void Mind::generate_thoughts() {}
void Mind::select_action() {}
void Mind::apply_action() {}
void Mind::update_emotion() {}
void Mind::store_experience() {}
void Mind::adjust_identity() {}