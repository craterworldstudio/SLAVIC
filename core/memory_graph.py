# core/memory_graph.py

def update_belief(memory, thought):
    beliefs = memory.get("belief_graph", {})

    if thought.count("Is '") > 2:
        return False

    # Decay all existing beliefs slightly
    for b in list(beliefs.keys()):
        beliefs[b]["confidence"] *= 0.995  # slow decay
        beliefs[b]["confidence"] = max(0.0, beliefs[b]["confidence"])

        if beliefs[b]["confidence"] < 0.05:
            del beliefs[b]
        

    # Detect conflict: if new thought contradicts existing belief
    conflict_detected = False
    for b in beliefs:
        if is_conflict(b, thought):
            beliefs[b]["confidence"] -= 0.1
            conflict_detected = True

    # Add/update the current thought
    if thought not in beliefs:
        beliefs[thought] = {"confidence": 0.5, "last_updated": 0}
    else:
        beliefs[thought]["confidence"] += 0.05

    # Clamp confidence
    if thought in beliefs:
        beliefs[thought]["confidence"] = max(0.0, min(1.0, beliefs[thought]["confidence"]))

    memory["belief_graph"] = beliefs
    return conflict_detected

def is_conflict(existing_belief, new_thought):
    """
    Minimal conflict detection:
    - checks negation words as a placeholder
    """
    negations = ["not", "never", "can't", "cannot"]
    for n in negations:
        if n in existing_belief and n not in new_thought:
            return True
        if n in new_thought and n not in existing_belief:
            return True
    return False