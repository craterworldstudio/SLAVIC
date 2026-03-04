#pragma once

#include <memory>

class Mind {
public:
    Mind();

    void tick();
    bool is_running() const;

    void load_state();
    void save_state();

private:
    int tick_count;
    bool running;

    void perceive();
    void update_drives();
    void generate_thoughts();
    void select_action();
    void apply_action();
    void update_emotion();
    void store_experience();
    void adjust_identity();
};