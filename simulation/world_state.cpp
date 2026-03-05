//world_state.cpp
#include "world_state.hpp"
#include <cstdlib>

void WorldState::update() {
    // very primitive fluctuation
    threat += ((rand() % 100) / 1000.0f) - 0.05f;
    resource += ((rand() % 100) / 1000.0f) - 0.05f;
    social_presence += ((rand() % 100) / 1000.0f) - 0.05f;

    if (threat < 0) threat = 0;
    if (resource < 0) resource = 0;
    if (social_presence < 0) social_presence = 0;

    if (threat > 1) threat = 1;
    if (resource > 1) resource = 1;
    if (social_presence > 1) social_presence = 1;
}