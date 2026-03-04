#core/scheduler.py
import time
import random
from core.cognition import process_thought_impact
from core.drives import update_drives
from core.cognition import generate_thought, generate_reflective_thought
from core.memory_graph import update_belief
from logging.state import load_state, save_state
from logging.memory import load_memory, save_memory
from logging.utils import log_diary
from core.world_sim import apply_action, drift_world
from core.predictive_model import initialize_model, predict_outcome, update_model
from core.self_model import SelfModel
from core.self_model import calculate_dissonance
from core.interaction import handle_interaction
from config import *

# Action mapping for each drive
ACTION_MAP = {
    "curiosity": ["explore_memory"],
    "coherence": ["reconcile_beliefs"],
    "stability": ["rehearse_habits"],
    "expansion": ["experiment"],
    "social": ["interact"],
    "nothing": ["do_nothing"]
}

def consolidate_memory(memory):
    """
    The 'Sleep' function: Prunes the belief graph and 
    strengthens high-confidence patterns.
    """
    beliefs = memory.get("belief_graph", {})
    # Remove anything the agent isn't sure about to lower 'Cognitive Load'
    to_delete = [b for b, data in beliefs.items() if data["confidence"] < 0.2]
    for b in to_delete:
        del beliefs[b]
    
    memory["belief_graph"] = beliefs
    return memory

def select_action(state):
    """
    Choose an action based on dominant drive and internal tension
    """
    dominant = state["dominant_drive"]

    if state["energy"] < 0.2:
        return random.choice(ACTION_MAP["stability"])
    
    #actions = ACTION_MAP.get(dominant, ["do_nothing"])

    if state["internal_tension"] > 0.5:
        # 30% chance to instinctively revert to habits to lower tension
        if random.random() < 0.3:
            return "rehearse_habits"
        
        # Otherwise, the agent acts erratically (picks any random drive)
        random_drive = random.choice(list(ACTION_MAP.keys()))
        return random.choice(ACTION_MAP[random_drive])
    
    actions = ACTION_MAP.get(dominant, ["rehearse_habits"])
    return random.choice(actions)

    #if state["energy"] < 0.3:
    #    state["internal_tension"] *= 0.5

    #if state["energy"] < 0.3:
    #    return random.choice(ACTION_MAP["stability"])
    #
    ## Internal tension may make agent pick a random alternative
    #.keys()))
    #    actions.extend(ACTION_MAP[random_drive])if state["internal_tension"] > 0.3:
    #    random_drive = random.choice(list(ACTION_MAP
    #
    #return random.choice(actions)

def run():
    tick_count = 0
    state = load_state()
    memory = load_memory()
    sm = SelfModel()
    #initialize_model(memory)

    repeat_count = 0
    last_logged_thought = ""
    action_repeat_count = 0
    last_action = None

    predictable_ticks = 0
    
    while True:
        state = load_state()
        memory = load_memory()
        memory = initialize_model(memory)
        
        sm.update_state(state)
        

        tick_count += 1 # type: ignore
        state["time"] += 1

        # 1️⃣ WORLD DRIFT (environment moves independently)
        state = drift_world(state)

        world = state["world"]

        # 2️⃣ WORLD → EMOTION COUPLING
        state["emotions"]["anxiety"] += world["threat_level"] * 0.02

        if world["resources"] < 2:
            state["energy"] -= 0.02

        if world["social_presence"] < 0.3:
            state["drives"]["social"] += 0.02

        # LONELINESS: If social presence is low, anxiety and social drive rise.
        if world["social_presence"] < 0.15:
            state["emotions"]["anxiety"] += 0.03
            state["drives"]["social"] += 0.05  # The "need" to interact grows
            
        # OVERSTIMULATION: If social presence is too high, it drains energy.
        if world["social_presence"] > 0.8 and state["drives"]["social"] < 0.3:
            state["energy"] -= 0.03
            state["internal_tension"] += 0.05

        sm.update_traits(state)

        # Clamp emotions early
        for e in state["emotions"]:
            state["emotions"][e] = max(0.0, min(1.0, state["emotions"][e]))

        # 3️⃣ Energy recovery
        state["energy"] = min(1.0, state["energy"] + RECOVERY_RATE)

        # 4️⃣ Update drives (now influenced by world pressure)
        state = update_drives(state)

        dissonance = calculate_dissonance(state)
        if dissonance > 0:
            state["internal_tension"] += dissonance
            state["emotions"]["anxiety"] += (dissonance * 0.5)
            log_diary("system_event", f"Cognitive Dissonance detected: {dissonance}", state)

        # 5️⃣ Generate thought
        thought = generate_thought(state, memory)
        state = process_thought_impact(state, thought)
        if thought == last_logged_thought:
            repeat_count += 1
        else:
            repeat_count = 0

        last_logged_thought = thought

        # If the agent repeats itself more than 3 times, internal tension spikes
        if repeat_count > 3:
            state["internal_tension"] += 0.05
            state["emotions"]["frustration"] += 0.05
            log_diary("system_event", "Repetitive thought detected. Increasing friction.", state)

        conflict = update_belief(memory, thought)
        

        if conflict:
            state["internal_tension"] += 0.1
            state["emotions"]["anxiety"] += 0.1
            cthought = f"Conflict detected: Internal tension rising. Thought: {thought}"
            state["last_thought"] = cthought
            log_diary("system_event", cthought, state)
        else:
            state["last_thought"] = thought
            log_diary("internal_thought", thought, state)

        # 6️⃣ Reflection/Consolidation cycle
        if tick_count % REFLECTION_INTERVAL == 0:
            reflective_thought = generate_reflective_thought(state, memory)
            update_belief(memory, reflective_thought)
            state["self_model"]["traits"] = sm.traits
            state["last_thought"] = sm.generate_narrative() if tick_count % 10 == 0 else state["last_thought"]
            log_diary("reflective_thought", reflective_thought, state)


        #CONSOLIDATION (The "Sleep" Flag)
        if tick_count % 50 == 0 or state["internal_tension"] > 0.6:
            memory = consolidate_memory(memory)
            state["internal_tension"] *= 0.5  # Artificial 'calming' effect
            state["internal_tension"] = max(0.0, state["internal_tension"])
            state["emotions"]["anxiety"] *= 0.8
            log_diary("system_event", "Consolidating memory and lowering tension.", state)

        # 7️⃣ Select action
        action = select_action(state)
        if action == last_action:
            action_repeat_count += 1
        else:
            action_repeat_count = 0

        last_action = action

        # Exploration Fatigue (Behavioral Diversification Mechanism)
        if action_repeat_count > 3:
            log_diary("system_event", "Behavioral fixation detected. Inducing diversification.", state)
        
            # Reduce dominant drive slightly
            state["drives"][state["dominant_drive"]] *= 0.8
        
            # Instead of random chaos, choose viable alternative
            viable_actions = [
                a for a, data in memory["action_model"].items()
                if data["success_rate"] > 0.2
            ]
        
            if viable_actions:
                action = random.choice(viable_actions)


        # 8️⃣ Predict outcome
        predicted = predict_outcome(memory, action)

        # 9️⃣ Apply action
        if action == "interact":
            outcome = handle_interaction(state, state["world"])
        else:
            state, outcome = apply_action(state, action, sm)

        #state, outcome = apply_action(state, action, sm)

        # 🔟 Energy cost
        state["energy"] -= ENERGY_COST
        state["energy"] = max(0.0, state["energy"])

        # 1️⃣1️⃣ Update predictive model
        error = update_model(memory, action, outcome, predicted)
        state["prediction_error"] = error

        # 🧠 Boredom Mechanism (Predictability -> Curiosity)
        if abs(state["prediction_error"]) < 0.1:
            #state["drives"]["curiosity"] += 0.02
            predictable_ticks = min(predictable_ticks + 1, 200)
        else:
            predictable_ticks = 0

        state["drives"]["curiosity"] += 0.0015 * predictable_ticks

        # 1️⃣2️⃣ Instability burst (phase transition)
        if abs(error) > 0.6:
            state["drives"]["curiosity"] += 0.1
            state["emotions"]["anxiety"] += 0.1

        # 1️⃣3️⃣ Error-based emotional shift
        if error < -0.4:
            state["emotions"]["frustration"] += 0.1
            state["emotions"]["confidence"] -= 0.1
        elif error > 0.4:
            state["emotions"]["confidence"] += 0.05

        # 1️⃣4️⃣ Recovery mode (low energy)
        if state["energy"] < 0.2:
            for d in state["drives"]:
                if d != "stability":
                    state["drives"][d] *= 0.7
            state["drives"]["stability"] += 0.1

        # Clamp drives and emotions again
        for d in state["drives"]:
            state["drives"][d] = max(0.0, min(1.0, state["drives"][d]))

        for e in state["emotions"]:
            state["emotions"][e] = max(0.0, min(1.0, state["emotions"][e]))

        # Emotional baseline drift (nervous system recovery)
        for e in state["emotions"]:
            baseline = 0.2  # resting emotional tone
            state["emotions"][e] += (baseline - state["emotions"][e]) * 0.02

        # 1️⃣5️⃣ Log action
        log_diary("action_taken", {"action": action, "outcome": outcome}, state)

        # 1️⃣6️⃣ Persist
        save_state(state)
        save_memory(memory)

        time.sleep(2)