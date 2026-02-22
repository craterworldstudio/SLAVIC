import random
from collections import defaultdict
from memory import load_memory, save_memory

ALPHABET = list("abcdefghijklmnopqrstuvwxyz ")
LEARNING_RATE = 0.3
DECAY_RATE = 0.001
EXPANSION_THRESHOLD = 0.8
MAX_GENERATION_LENGTH = 6


class ExpandingMind:
    def __init__(self):
        memory = load_memory()
        self.model = memory.get("symbol_model", {})

    def _save_model(self):
        memory = load_memory()
        memory["symbol_model"] = self.model
        save_memory(memory)

    def _get_contexts(self, sequence):
        return [sequence[i:] for i in range(len(sequence))]

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

    def generate(self):
        sequence = ""
        for _ in range(MAX_GENERATION_LENGTH):
            next_char = self._choose_next_char(sequence)
            sequence += next_char
        return sequence

    def reward(self, sequence, reward_value):
        for i in range(len(sequence)):
            context = sequence[:i]
            next_char = sequence[i]

            if context in self.model:
                self.model[context]["transitions"][next_char] += LEARNING_RATE * reward_value

        if reward_value >= EXPANSION_THRESHOLD:
            for i in range(len(sequence)):
                context = sequence[:i]
                next_char = sequence[i]

                if context not in self.model:
                    self.model[context] = {
                        "transitions": defaultdict(lambda: 0.1),
                        "value": 0.0
                    }

                if next_char not in self.model[context]["transitions"]:
                    self.model[context]["transitions"][next_char] = 0.1

                self.model[context]["value"] += reward_value

        self._decay()
        self._save_model()

    def _decay(self):
        for ctx in self.model:
            for char in self.model[ctx]["transitions"]:
                self.model[ctx]["transitions"][char] *= (1 - DECAY_RATE)
