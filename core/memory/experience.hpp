#pragma once
#include "../types.hpp"

struct Experience
{
    ActionType action;
    float outcome;
    float tension;
    float novelty;
    float emotional_intensity;

    Experience(ActionType a,
               float o,
               float t,
               float n,
               float e)
        : action(a),
          outcome(o),
          tension(t),
          novelty(n),
          emotional_intensity(e) {}
};