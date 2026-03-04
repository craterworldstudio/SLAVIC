import time, random, string
from brain import ExpandingMind
from memory import load_memory, save_memory
from state import load_state, save_state, drift_state
from autonomy import generate_internal_thought
from utils import log_diary
from config import *

class RLWLM:
    def __init__(self) -> None:
        self.RLWORDs = []

    def reset(self):
        self.RLWORDs = WORDS
    
    def update_RLWL(self, LwL):
        self.RLWORDs = [wd for wd in WORDS if wd not in LwL]

def should_generate_thought(state):
    return random.random() < state["curiosity"] * state["energy"]

with open("database/words1000.txt") as f:
    WORDS = set(word.strip().lower() for word in f)
    print("[!] List prepared.")
    

def is_clean_word(sequence):
    token = sequence.strip().lower()

    if token in WORDS:
        return True

    # reject if it contains a known word but extra chars
    for word in WORDS:
        if word in token and word != token:
            return False

    return False

def evaluate_learning(sequence, state):
    phase = state["learning_phase"]

    if phase == "alphabet":
        if len(sequence.strip()) == 1 and sequence.strip() in string.ascii_lowercase:
            reward = 1.0
        else:
            reward = 0.05

    elif phase == "words":
        clean = sequence.strip().lower()
        reward = 0.0

        if clean in WORDS:
            if len(clean) == 1 and clean not in VALID_SHORT:
                reward = 0.0
            else:
                if len(clean) == 1:
                    reward = 0.4   # weaker reward for single letters
                else:
                    reward = 1.0
        else:
            reward = 0.0

        #if any(v in clean for v in "aeiou"):
        #    reward += 0.1
        #if 3 <= len(clean) <= 6:
        #    reward += 0.1

    elif phase == "sentences":
        if " " in sequence.strip() and len(sequence.split()) >= 2:
            reward = 1.0
        else:
            reward = 0.05

    else:
        reward = 0.01

    # frustration update
    # frustration rises on failure
    if reward < 0.5: state["frustration"] += (0.5 - reward) * 0.1
    # frustration drops on strong success
    else: state["frustration"] -= reward * 0.05
    # small passive recovery over time
    state["frustration"] *= 0.99
    state["frustration"] = max(0.0, min(1.0, state["frustration"]))

    return reward

def update_competence(state, reward, clean_structure):
    phase = state["learning_phase"]

    if reward > 0.8 and clean_structure:
        state["competence"][phase] += 0.02

    elif reward < 0.2:
        penalty = 0.02 * (1 - state["competence"][phase])
        state["competence"][phase] -= penalty
        #state["competence"][phase] -= 0.02

    state["competence"][phase] = max(0.0, min(1.0, state["competence"][phase]))

def check_phase_progression(state):
    phase = state["learning_phase"]

    if phase == "alphabet" and state["competence"]["alphabet"] > 0.6:
        state["learning_phase"] = "words"
        print(">>> Transitioning to WORDS phase")

    #elif ( phase == "words" and state["competence"]["words"] > 0.8 and state["frustration"] < 0.3):
    #    state["learning_phase"] = "sentences"
    #    print(">>> Transitioning to SENTENCES phase")

def run_autonomy_loop():
    mind = ExpandingMind()
    rlwm = RLWLM()

    while True:
        state = load_state()
        state["generation"] = state.get("generation", 0) + 1
        state = drift_state(state)
        recent_outputs = state.get("recent_outputs", [])
        

        if "competence_history" not in state:
            state["competence_history"] = []

        memory = load_memory()
        vocab = memory.get("learned_vocabulary", {})

        # Prepare RL word list
        rlwm.update_RLWL(list(vocab.keys()))
        if not rlwm.RLWORDs:
            rlwm.reset()

        phase = state["learning_phase"]
        inj = False
        state["LWC"] = len(vocab.keys())

        # ===============================
        # ALPHABET PHASE
        # ===============================
        if phase == "alphabet":
            sequence = mind.generate_single()

        # ===============================
        # WORDS PHASE (DETERMINISTIC TRAINING)
        # ===============================
        elif phase == "words":

            # If there are still unseen words, train on them directly
            if rlwm.RLWORDs:
                sequence = random.choice(list(rlwm.RLWORDs))
                inj = True
            else:
                # All words seen → test free generation
                sequence = mind.generate()

        # ===============================
        # SENTENCES PHASE
        # ===============================
        else:
            sequence = mind.generate()

        reward = evaluate_learning(sequence, state)

        recent_outputs.append(sequence)

        if len(recent_outputs) > 10:
            recent_outputs = recent_outputs[-10:]

        state["recent_outputs"] = recent_outputs


        # ===============================
        # STRONGER NEGATIVE REINFORCEMENT
        # ===============================
        if reward < 0.5:
            effective_reward = -0.2
        else:
            effective_reward = reward

        mind.reward(sequence, effective_reward)

        clean_structure = is_clean_word(sequence)

        generation = state["generation"]

        # ===============================
        # SAVE VOCAB LEARNING
        # ===============================
        if (
            phase == "words"
            and reward >= 1.0
            and clean_structure
        ):
            word = sequence.strip().lower()

            if word not in vocab:
                vocab[word] = {
                    "count": 1,
                    "first_seen": generation,
                    "last_seen": generation
                }
            else:
                vocab[word]["count"] += 1
                vocab[word]["last_seen"] = generation

            memory["learned_vocabulary"] = vocab
            memory["RLV"] = rlwm.RLWORDs
            save_memory(memory)

        # ===============================
        # UPDATE COMPETENCE
        # ===============================
        update_competence(state, reward, clean_structure)

        state["competence_history"].append(state["competence"]["words"])
        if len(state["competence_history"]) > MAX_HISTORY:
            state["competence_history"] = state["competence_history"][-MAX_HISTORY:]

        # ===============================
        # PHASE TRANSITION CHECK
        # ===============================
        if phase == "words" and len(vocab) >= len(WORDS) * 0.95:
            state["learning_phase"] = "sentences"
            print(">>> Vocabulary mostly learned. Transitioning to SENTENCES phase.")

        check_phase_progression(state)

        log_diary(
            f"symbol_generation INJ: {inj}",
            f"{sequence} | reward={reward:.2f}",
            state
        )

        save_state(state)

        time.sleep(random.randint(
            THOUGHT_INTERVAL_MIN,
            THOUGHT_INTERVAL_MAX
        ))