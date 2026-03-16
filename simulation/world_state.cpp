//world_state.cpp
#include "world_state.hpp"
#include <cstdlib>

void WorldState::update() {
    // very primitive fluctuation
    prev_threat = threat;
    prev_resource = resource;
    prev_social = social_presence;

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

float WorldState::get_novelty() const
{
    float d1 = std::abs(threat - prev_threat);
    float d2 = std::abs(resource - prev_resource);
    float d3 = std::abs(social_presence - prev_social);

    return (d1 + d2 + d3) / 3.0f;
}