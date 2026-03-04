#logging/memory.py
import json

MEMORY_FILE = "memory.json"

def load_memory():
    with open(MEMORY_FILE, "r") as f:
        return json.load(f)

def save_memory(memory):
    with open(MEMORY_FILE, "w") as f:
        json.dump(memory, f, indent=2)

def add_short_term(thought):
    memory = load_memory()
    memory["short_term"].append(thought)
    save_memory(memory)