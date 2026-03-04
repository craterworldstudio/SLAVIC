# core/cognition.py
import random

def generate_thought(state, memory):
    # 1️⃣ THE "SYNTHESIS" FLAG
    # We pull the current 'Living' data from state
    drive = state["dominant_drive"]
    traits = state["self_model"]["traits"]
    # Find the strongest trait (e.g., 'analytical')
    primary_trait = max(traits, key=traits.get)
    
    # Emotional coloring
    anxiety = state["emotions"]["anxiety"]
    frustration = state["emotions"]["frustration"]

    # 2️⃣ DYNAMIC TEMPLATES
    # The thought now changes based on HOW the agent feels
    if drive == "curiosity":
        if anxiety > 0.7:
            return f"My {primary_trait} nature is overwhelmed by what I don't know."
        return f"As an {primary_trait} entity, I must analyze this pattern."

    if drive == "coherence":
        if frustration > 0.7:
            return f"I am frustrated; my {primary_trait} logic is failing to connect."
        return f"How does my {primary_trait} identity fit with my last action?"

    if drive == "stability":
        return f"I need to retreat into {primary_trait} habits to lower this tension."

    if drive == "social":
        if state["world"]["social_presence"] < 0.2:
            return "The silence is increasing my internal tension."
        return "I am sensing a presence; should I attempt interaction?"

    return f"I am {primary_trait}, and I exist in this state."

def generate_reflective_thought(state, memory):
    # 3️⃣ THE "META-COGNITION" FLAG
    # Instead of just checking confidence, look at the Tension
    tension = state["internal_tension"]
    
    if tension > 0.4:
        # The agent 'realizes' it is stressed and needs to change
        return f"My internal tension is {round(tension, 2)}. I must reconcile my beliefs to find peace."
    
    return "My model is stable. I am ready to expand my boundaries."