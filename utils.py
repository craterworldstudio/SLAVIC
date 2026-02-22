import time

def log_diary(entry_type, content, state):
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

    entry = (
        f"[{timestamp}] TYPE: {entry_type}\n"
        f"Energy: {state['energy']:.2f} | Mood: {state['mood']:.2f}\n"
        f"Content: {content}\n\n"
    )

    with open("diary.log", "a") as f:
        f.write(entry)