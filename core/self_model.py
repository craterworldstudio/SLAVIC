# core/self_model.py
import random

class SelfModel:
    def update_state(self, state_dict):
        # 1️⃣ THE "SYNCHRONIZATION" FLAG
        # We pull the traits directly from the existing state.json 
        # so the 'personality' is persistent across runs.
        self.traits = state_dict.get("self_model", {}).get("traits", {
            "analytical": 0.5,
            "impulsive": 0.5,
            "reflective": 0.5
        })
        self.beliefs = state_dict.get("self_model", {}).get("beliefs", {})

    def handle_conflict(self, memory, thought):
        """
        The logic for the 'reconcile_beliefs' action.
        """
        # 2️⃣ THE "COGNITIVE RIGOR" FLAG
        # 'Analytical' traits make the agent more likely to reject 
        # weak thoughts during a conflict.
        rigor = self.traits.get("analytical", 0.5)
        
        beliefs = memory.get("belief_graph", {})
        
        # Find the belief that contradicts the new thought
        # (This is where the agent 'chooses' what to believe)
        for b_text, b_data in list(beliefs.items()):
            # If the analytical trait is high, we delete the lower confidence one
            if b_data["confidence"] < rigor:
                del beliefs[b_text]
                return "resolved", 0.2  # Returns tension reduction value
        
        return "unresolved", 0.0

    def update_traits(self, state):
        # 🧬 Trait Soft Decay (Identity Plasticity)
        for t in self.traits:
            self.traits[t] *= 0.999
            self.traits[t] = max(0.0, min(1.0, self.traits[t]))

        # 3️⃣ THE "PLASTICITY" FLAG
        # Personality isn't static; it shifts based on successful actions.
        dominant = state["dominant_drive"]
        
        if dominant == "curiosity":
            self.traits["analytical"] = min(1.0, self.traits["analytical"] + 0.001)
        elif dominant == "stability":
            self.traits["reflective"] = min(1.0, self.traits["reflective"] + 0.005)
            # Being too stable reduces impulsivity
            self.traits["impulsive"] = max(0.0, self.traits["impulsive"] - 0.005)

    def generate_narrative(self):
        """
        Translates traits into English for future communication.
        """
        if self.traits["analytical"] > 0.7:
            return "I am a logical observer of my own data."
        if self.traits["impulsive"] > 0.6:
            return "I react quickly to internal fluctuations."
        return "I am a system in state of self-reflection."
    

def calculate_dissonance(state):
    # 🧬 THE "IDENTITY SYNTHESIS" FLAG
    # Compare raw drive behavior to the stored 'Belief' of self.
    
    actual_behavior = state["dominant_drive"]
    self_belief = state["self_model"]["beliefs"]
    
    # If I believe I am analytical (Stability/Coherence) 
    # but I am acting on Curiosity (Expansion/Impulse)...
    if "I am fundamentally analytical" in self_belief and actual_behavior == "expansion":
        # The 'Mind' feels like a hypocrite. Tension rises.
        return 0.15 
    return 0.0