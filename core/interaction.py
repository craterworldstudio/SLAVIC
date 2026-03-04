#core/interaction.py

import random
import math

# ----------------------------
# Social Environment Definition
# ----------------------------

SOCIAL_AGENTS = {
    "User": {
        "warmth": 0.75,
        "reliability": 0.85
    },
    "Stranger": {
        "warmth": 0.45,
        "reliability": 0.50
    },
    "DistantAgent": {
        "warmth": 0.25,
        "reliability": 0.35
    }
}

# Persistent trust memory
trust_levels = {
    name: 0.4 for name in SOCIAL_AGENTS.keys()
}

# ----------------------------
# Utility
# ----------------------------

def clamp(x, lo=0.0, hi=1.0):
    return max(lo, min(hi, x))

# ----------------------------
# Main Interaction Logic
# ----------------------------

def handle_interaction(state, world):
    """
    Called when scheduler selects 'interact'.
    Returns: "success" or "failure"
    """

    # Choose agent probabilistically weighted by trust
    agent_names = list(SOCIAL_AGENTS.keys())
    weights = [trust_levels[name] + 0.01 for name in agent_names]
    total = sum(weights)
    weights = [w / total for w in weights]

    target = random.choices(agent_names, weights=weights)[0]

    warmth = SOCIAL_AGENTS[target]["warmth"]
    reliability = SOCIAL_AGENTS[target]["reliability"]
    trust = trust_levels[target]

    # Success probability formula
    success_probability = (
        (warmth * 0.4) +
        (reliability * 0.3) +
        (trust * 0.3)
    )

    # Environmental modifier
    success_probability *= (0.5 + world.get("social_presence", 0.1))

    success_probability = clamp(success_probability)

    if random.random() < success_probability:
        # Success path
        trust_levels[target] = clamp(trust + 0.03)

        state["emotions"]["confidence"] = clamp(
            state["emotions"].get("confidence", 0) + 0.05
        )

        state["emotions"]["anxiety"] = clamp(
            state["emotions"].get("anxiety", 0) - 0.04
        )

        world["social_presence"] = clamp(
            world.get("social_presence", 0.1) + 0.08
        )

        return "success"

    else:
        # Failure path (gentle decay, not collapse)
        trust_levels[target] = clamp(trust - 0.01)

        state["emotions"]["anxiety"] = clamp(
            state["emotions"].get("anxiety", 0) + 0.03
        )

        world["social_presence"] = clamp(
            world.get("social_presence", 0.1) - 0.02
        )

        return "failure"