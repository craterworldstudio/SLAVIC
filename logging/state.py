#logging/state.py
import json

STATE_FILE = "state.json"

def load_state():
    with open(STATE_FILE, "r") as f:
        return json.load(f)

def save_state(state):
    with open(STATE_FILE, "w") as f:
        json.dump(state, f, indent=2)

def clamp(val):
    return max(0.0, min(1.0, val))

from config import ENERGY_DECAY, MOOD_DRIFT
import random

def drift_state(state):
    state["energy"] = clamp(state["energy"] - ENERGY_DECAY) #random.uniform(-ENERGY_DECAY, ENERGY_DECAY))
    state["mood"] = clamp(state["mood"] + random.uniform(-MOOD_DRIFT, MOOD_DRIFT))
    return state