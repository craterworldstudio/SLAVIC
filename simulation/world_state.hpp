//world_state.hpp
#pragma once

struct WorldState {
public:
    float threat = 0.2f;
    float resource = 0.8f;
    float social_presence = 0.3f;

    void update();
    float get_novelty() const;

private:
    float prev_threat = 0.5f;
    float prev_resource = 0.5f;
    float prev_social = 0.5f;
};