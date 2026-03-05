//drive_system.cpp
#include "drive_system.hpp"
#include <algorithm>

DriveSystem::DriveSystem() {
    drives["curiosity"] = 0.5f;
    drives["stability"] = 0.5f;
    drives["social"] = 0.5f;
    drives["coherence"] = 0.5f;
    drives["expansion"] = 0.5f;
}

void DriveSystem::update(float threat, float resource, float social_presence) {

    drives["stability"] = 1.0f - threat;
    drives["curiosity"] = 1.0f - resource;
    drives["social"] = social_presence;
    drives["coherence"] = 1.0f - std::abs(threat - resource);
    drives["expansion"] = (drives["curiosity"] + drives["social"]) / 2.0f;

    for (auto& d : drives)
        clamp(d.second);
}

float DriveSystem::get_drive(const std::string& name) const {
    return drives.at(name);
}

float DriveSystem::get_internal_tension() const {

    float min_val = 1.0f;
    float max_val = 0.0f;

    for (const auto& d : drives) {
        min_val = std::min(min_val, d.second);
        max_val = std::max(max_val, d.second);
    }

    return max_val - min_val;
}

void DriveSystem::clamp(float& value) {
    value = std::clamp(value, 0.0f, 1.0f);
}