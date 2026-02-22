import time
import random
from brain import ExpandingMind
from state import load_state, save_state, drift_state
from autonomy import generate_internal_thought
from utils import log_diary
from config import THOUGHT_INTERVAL_MIN, THOUGHT_INTERVAL_MAX

def should_generate_thought(state):
    return random.random() < state["curiosity"] * state["energy"]

"""def run_autonomy_loop():
    while True:
        state = load_state()

        state = drift_state(state)

        

        if state["energy"] <= 0.2:
            sleep_duration = random.randint(3, 5)  # long sleep
            log_diary("sleep_start", f"Sleeping for {sleep_duration} seconds.", state)

            time.sleep(sleep_duration)

            state["energy"] = min(1.0, state["energy"] + 0.7)

            log_diary("sleep_end", "Woke up feeling restored.", state)

        thought = None
        if should_generate_thought(state):
            thought = generate_internal_thought(state)
            log_diary("internal_thought", thought, state)

        save_state(state)

        sleep_time = random.randint(
            THOUGHT_INTERVAL_MIN,
            THOUGHT_INTERVAL_MAX
        )

        time.sleep(sleep_time)
"""

import string

with open("word_list.txt") as f:
    WORDS = set(word.strip().lower() for word in f)

def evaluate_learning(sequence, state):
    phase = state["learning_phase"]

    if phase == "alphabet":
        if len(sequence.strip()) == 1 and sequence.strip() in string.ascii_lowercase:
            reward = 1.0
        else:
            reward = 0.05

    elif phase == "words":
        #words = ["cat", "dog", "sun", "sky"]
        reward = 1.0 if sequence.strip().lower() in WORDS else 0.02

    elif phase == "sentences":
        if " " in sequence.strip() and len(sequence.split()) >= 2:
            reward = 1.0
        else:
            reward = 0.05

    else:
        reward = 0.01

    # frustration update
    state["frustration"] += (1 - reward) * 0.1
    state["frustration"] = max(0.0, min(1.0, state["frustration"]))

    return reward

def update_competence(state, reward):
    phase = state["learning_phase"]

    # competence increases with consistent reward
    state["competence"][phase] += reward * 0.02
    state["competence"][phase] = min(1.0, state["competence"][phase])

def check_phase_progression(state):
    phase = state["learning_phase"]

    if phase == "alphabet" and state["competence"]["alphabet"] > 0.6:
        state["learning_phase"] = "words"
        print(">>> Transitioning to WORDS phase")

    elif phase == "words" and state["competence"]["words"] > 0.6:
        state["learning_phase"] = "sentences"
        print(">>> Transitioning to SENTENCES phase")

def run_autonomy_loop():
    mind = ExpandingMind()

    while True:
        state = load_state()
        state = drift_state(state)

        sequence = mind.generate()

        reward = evaluate_learning(sequence, state)

        mind.reward(sequence, reward)

        update_competence(state, reward)

        check_phase_progression(state)

        log_diary("symbol_generation", f"{sequence} | reward={reward:.2f}", state)

        save_state(state)

        sleep_time = random.randint(
            THOUGHT_INTERVAL_MIN,
            THOUGHT_INTERVAL_MAX
        )

        time.sleep(sleep_time)