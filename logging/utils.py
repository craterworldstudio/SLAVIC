#logging/utils.py
import time

def log_diary(entry_type, content, state):
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

    energy = state.get("energy", 0.0)
    mood = state.get("mood", 0.0)

    entry = (
        f"[{timestamp}] TYPE: {entry_type}\n"
        f"Energy: {energy:.2f} | Mood: {mood:.2f}\n"
        f"Content: {content}\n\n"
    )

    with open("diary.log", "a") as f:
        f.write(entry)
