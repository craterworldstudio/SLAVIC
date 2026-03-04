import random
from collections import defaultdict
from memory import load_memory, save_memory
from config import *


class ExpandingMind:
    def __init__(self):
        memory = load_memory()
        raw_model = memory.get("symbol_model", {})

        self.model = {}

        for ctx, data in raw_model.items():
            if ctx == "": ctx = ROOT

            self.model[ctx] = {
                "transitions": defaultdict(lambda: 0.1, data["transitions"]),
                "value": data.get("value", 0.0)
            }

    def _save_model(self):
        memory = load_memory()
        memory["symbol_model"] = self.model
        save_memory(memory)

    #def _get_contexts(self, sequence):
    #    return [sequence[i:] for i in range(len(sequence))]
    
    def _get_contexts(self, sequence):
        if not sequence.startswith(ROOT):
            sequence = ROOT + sequence

        contexts = []
        base = sequence[len(ROOT):]

        contexts.append(ROOT + base)

        for i in range(len(base)):
            contexts.append(ROOT + base[i:])

        return contexts

    def _weighted_choice(self, transitions):
        total = sum(transitions.values())
        r = random.uniform(0, total)
        upto = 0
        for char, weight in transitions.items():
            upto += weight
            if upto >= r:
                return char
        return random.choice(ALPHABET)

    def _choose_next_char(self, context):
        contexts = self._get_contexts(context)

        for ctx in contexts:
            if ctx in self.model and self.model[ctx]["transitions"]:
                return self._weighted_choice(self.model[ctx]["transitions"])

        return random.choice(ALPHABET)
    
    def generate_single(self):
        return random.choice(ALPHABET)

    def generate(self):
        length = random.randint(MIN_LENGTH, MAX_LENGTH)
        context = ROOT
        output = ""

        for _ in range(length):
            #if len(output) >= MIN_LENGTH and random.random() < 0.2:
            #   break
            next_char = self._choose_next_char(context) 

            if next_char == END_TOKEN:
                break
            output += next_char
            context = context + next_char
        
        return output

    def reward(self, sequence, reward_value):
        for i in range(len(sequence)):
            context = ROOT + sequence[:i]
            next_char = sequence[i]

            if context in self.model:
                transitions = self.model[context]["transitions"]

                if next_char not in transitions: transitions[next_char] = 0.1

                # Positive reinforcement
                if reward_value > 0.5:
                    transitions[next_char] += LEARNING_RATE * reward_value
                # Mild negative reinforcement
                else:
                    transitions[next_char] -= LEARNING_RATE * 0.05
                    transitions[next_char] = max(0.01, transitions[next_char])

        if reward_value >= EXPANSION_THRESHOLD:
            for i in range(len(sequence)):
                context = ROOT + sequence[:i]
                next_char = sequence[i]

                if context not in self.model:
                    self.model[context] = {
                        "transitions": defaultdict(lambda: 0.1),
                        "value": 0.0
                    }

                if next_char not in self.model[context]["transitions"]:
                    self.model[context]["transitions"][next_char] = 0.1

                self.model[context]["value"] += reward_value

        final_context = ROOT + sequence
        if final_context not in self.model:
            self.model[final_context] = {
                "transitions": defaultdict(lambda: 0.1),
                "value": 0.0
            }

        transitions = self.model[final_context]["transitions"]

        if END_TOKEN not in transitions:
            transitions[END_TOKEN] = 0.1

        if reward_value > 0.5:
            transitions[END_TOKEN] += LEARNING_RATE * reward_value
            
        self._decay()
        self._save_model()

    def _decay(self):
        for ctx in self.model:
            transitions = self.model[ctx]["transitions"]

            # decay
            for char in transitions:
                transitions[char] *= (1 - DECAY_RATE)
                transitions[char] = max(0.01, transitions[char])

            # normalize
            total = sum(transitions.values())
            if total > 0:
                for char in transitions:
                    transitions[char] /= total
