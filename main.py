# main.py
import threading
from core.scheduler import run
from ui.visualizer import run_visualizer
from logging.state import load_state, save_state

class StateManager:
    """Provides the visualizer access to the live mind data."""
    def load_state(self):
        return load_state()
    def save_state(self, s):
        save_state(s)

if __name__ == "__main__":
    sm = StateManager()

    # 🧵 THE "OBSERVATION DECK" FLAG
    # We launch the 'Soul' (Logic) in a background thread.
    print("Initializing Digital Mind...")
    logic_thread = threading.Thread(target=run, daemon=True)
    logic_thread.start()

    # We launch the 'Visual World' in the main thread.
    print("Opening Manifestation Window...")
    run_visualizer(sm)