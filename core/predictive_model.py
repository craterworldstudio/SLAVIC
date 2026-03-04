#core/predictive_model.py
import random

def initialize_model(memory):
    if "action_model" not in memory:
        memory["action_model"] = {
            "explore_memory": {"success_rate": 0.5},
            "reconcile_beliefs": {"success_rate": 0.5},
            "rehearse_habits": {"success_rate": 0.8},
            "experiment": {"success_rate": 0.5},
            "interact": {"success_rate": 0.5},
            "do_nothing": {"success_rate":0.5}
        }
    
    return memory

def predict_outcome(memory, action):
    model = memory["action_model"]
    return model[action]["success_rate"]

def update_model(memory, action, actual_outcome, prediction, learning_rate=0.1):
    model = memory["action_model"]
    #prediction = model[action]["success_rate"]

    actual = 1.0 if actual_outcome == "success" else 0.0
    error = actual - prediction

    # simple delta rule
    model[action]["success_rate"] += learning_rate * error

    # clamp
    model[action]["success_rate"] = max(0.0, min(1.0, model[action]["success_rate"]))

    return error