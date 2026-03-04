#core/world_sim.py
import random
from core.self_model import SelfModel

# Minimal environment state
"""world_state = {
    "resources": 5,        # arbitrary resource points
    "threat_level": 0.3,   # 0.0 to 1.0
    "social_presence": 0.2 # 0.0 to 1.0
}"""

def drift_world(state):
    world = state["world"]

    world["threat_level"] += random.uniform(-0.02, 0.02)
    world["social_presence"] += random.uniform(-0.01, 0.01)

    # 🌱 Resource regeneration
    if world["resources"] < 10:
        world["resources"] += random.uniform(-0.15, 0.15)

    # clamp
    world["threat_level"] = max(0.0, min(1.0, world["threat_level"]))
    world["social_presence"] = max(0.0, min(1.0, world["social_presence"]))
    world["resources"] = max(0.0, min(10.0, world["resources"]))

    return state


def apply_action(state, action, sm):
    """
    Apply an action from the agent to the world.
    Non-deterministic consequences affect drives, emotions, and world state.
    """
    world_state = state["world"]
    outcome = random.choices(
        ["success", "failure"],
        weights=[0.7, 0.3],
        k=1
    )[0]

    if action == "explore_memory":
        if outcome == "success":
            state["drives"]["curiosity"] -= 0.08
            state["emotions"]["confidence"] += 0.03
            
            # 1️⃣ THE "INSIGHT" FLAG
            # Instead of just numbers, we add a concrete 'belief' about the self
            trait = max(state["self_model"]["traits"], key=state["self_model"]["traits"].get)
            new_belief = f"I am fundamentally {trait}."
            
            # Store it in the self_model if it's not already there
            if new_belief not in state["self_model"]["beliefs"]:
                state["self_model"]["beliefs"][new_belief] = round(state["emotions"]["confidence"], 2)
        else:
            state["drives"]["curiosity"] += 0.02
            state["emotions"]["frustration"] += 0.04
            state["prediction_error"] += 0.1

    elif action == "reconcile_beliefs":
        # 2️⃣ THE "RESOLUTION" FLAG
        # This now triggers the logic we discussed for the SelfModel class
        sm.update_state(state)
        
        # We try to 'resolve' one of the beliefs in memory
        # We pass the memory graph to see if we can prune weak nodes
        if outcome == "success":
            from logging.memory import load_memory, save_memory
            memory = load_memory()

            status, tension_relief = sm.handle_conflict(memory, state["last_thought"])

            if status == "resolved":
                save_memory(memory) # Save the 'cleaned' memory
                state["internal_tension"] -= tension_relief
                state["emotions"]["anxiety"] -= 0.1
                if "Maintain Consistency" not in state["self_model"]["goals"]:
                    state["self_model"]["goals"].append("Maintain Consistency")


            #state["internal_tension"] *= 0.7  # Significant relief
            #state["emotions"]["anxiety"] -= 0.1
            ## Logic: Successful reconciliation clears the 'last_thought' conflict
            #state["self_model"]["goals"].append("Maintain Consistency")
        else:
            state["internal_tension"] += 0.05

    elif action == "rehearse_habits":
        # 3️⃣ THE "GROUNDING" FLAG
        state["drives"]["stability"] += 0.05
        state["energy"] = min(1.0, state["energy"] + 0.3)
        # Rehearsing habits reinforces current beliefs
        for b in state["self_model"]["beliefs"]:
            state["self_model"]["beliefs"][b] = min(1.0, state["self_model"]["beliefs"][b] + 0.01)

    elif action == "experiment":
        state["drives"]["expansion"] += 0.04
        state["emotions"]["confidence"] += 0.02
        if outcome == "success":
            world_state["resources"] += random.uniform(0.5, 1.5)
        else:
            world_state["resources"] -= random.uniform(0.5, 1.0)

    elif action == "interact":
        # 3️⃣ THE "CONNECTION" FLAG
        # Interaction is only successful if there is actually someone there.
        if world_state["social_presence"] > 0.1:
            outcome = "success"
            state["drives"]["social"] -= 0.2  # Drive is satisfied
            state["emotions"]["frustration"] -= 0.1
            state["emotions"]["confidence"] += 0.05
            state["internal_tension"] *= 0.8  # Socializing lowers stress
        else:
            # Trying to interact when no one is there causes rejection/failure
            outcome = "failure"
            state["emotions"]["frustration"] += 0.1
            state["internal_tension"] += 0.1

    # Clamp drives and emotions
    for k in state["drives"]:
        state["drives"][k] = min(1.0, max(0.0, state["drives"][k]))
    for k in state["emotions"]:
        state["emotions"][k] = min(1.0, max(0.0, state["emotions"][k]))

    return state, outcome