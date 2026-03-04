#core/drives.py
import random
from config import *

def update_drives(state):
    drives = state["drives"]
    baseline = state["drive_baseline"]
    emotions = state["emotions"]

    # 1️⃣ Baseline gravity
    for d in drives:
        drives[d] += (baseline[d] - drives[d]) * BASELINE_PULL

    # 2️⃣ Emotional modulation
    drives["curiosity"] += abs(state["prediction_error"]) * 0.1
    drives["coherence"] += emotions["anxiety"] * 0.01
    drives["expansion"] += emotions["confidence"] * 0.1

    if abs(state.get("prediction_error", 0)) > 0.3:
        drives["curiosity"] += 0.05

    # Clamp after additive effects
    for d in drives:
        drives[d] = max(0.0, min(1.0, drives[d]))

    # 3️⃣ Select dominant
    dominant = max(drives, key=drives.get)
    state["dominant_drive"] = dominant

    # 4️⃣ Activation cost
    drives[dominant] -= ACTIVATION_COST

    # 5️⃣ Small recovery for others
    for d in drives:
        if d != dominant:
            drives[d] += 0.01

    # 6️⃣ Nonlinear ceiling
    for d in drives:
        drives[d] *= (1 - 0.3 * drives[d])

    # 7️⃣ Final clamp
    for d in drives:
        drives[d] = max(0.0, min(1.0, drives[d]))

    # 8️⃣ Internal tension (after all transformations)
    values = list(drives.values())
    state["internal_tension"] = max(values) - min(values)

    return state