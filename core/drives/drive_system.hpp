//drive_system.hpp
#pragma once
#include <unordered_map>
#include <string>

class DriveSystem {
public:
    DriveSystem();

    void update(float threat, float resource, float social_presence);
    float get_drive(const std::string& name) const;
    float get_internal_tension() const;
    void reduce_tension(float amount);

private:
    std::unordered_map<std::string, float> drives;

    void clamp(float& value);
};