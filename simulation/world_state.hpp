#pragma once

struct WorldState {
    float threat = 0.2f;
    float resource = 0.8f;
    float social_presence = 0.3f;

    void update();
};