import matplotlib.pyplot as plt
import json
import time
from collections import deque

STATE_FILE = "state.json"
MEMORY_FILE = "memory.json"

plt.ion()

fig, axs = plt.subplots(3, 1, figsize=(8, 10))

# Store history (last N ticks)
MAX_POINTS = 100

drive_history = {}
success_history = {}
tension_history = deque(maxlen=MAX_POINTS)
time_history = deque(maxlen=MAX_POINTS)

def load():
    with open(STATE_FILE) as f:
        state = json.load(f)
    with open(MEMORY_FILE) as f:
        memory = json.load(f)
    return state, memory

while True:
    state, memory = load()
    current_time = state.get("time", 0)

    drives = state["drives"]
    action_model = memory.get("action_model", {})

    # Initialize history keys if first run
    for d in drives:
        drive_history.setdefault(d, deque(maxlen=MAX_POINTS))
    for a in action_model:
        success_history.setdefault(a, deque(maxlen=MAX_POINTS))

    # Append new values
    time_history.append(current_time)

    for d, v in drives.items():
        drive_history[d].append(v)

    for a, v in action_model.items():
        success_history[a].append(v["success_rate"])

    tension_history.append(state.get("internal_tension", 0))

    # --- Plot Drives ---
    axs[0].clear()
    for d in drive_history:
        axs[0].plot(time_history, drive_history[d], label=d)
    axs[0].set_title("Drive Levels Over Time")
    axs[0].legend(loc="upper left")

    # --- Plot Action Success Rates ---
    axs[1].clear()
    for a in success_history:
        axs[1].plot(time_history, success_history[a], label=a)
    axs[1].set_title("Predicted Success Rates Over Time")
    axs[1].legend(loc="upper left")

    # --- Plot Internal Tension ---
    axs[2].clear()
    axs[2].plot(time_history, tension_history)
    axs[2].set_title("Internal Tension Over Time")

    plt.tight_layout()
    plt.pause(1)