import json
import matplotlib.pyplot as plt

with open("state.json") as f:
    state = json.load(f)

history = state["competence_history"]

plt.plot(history)
plt.title("Word Competence Over Time")
plt.xlabel("Generation")
plt.ylabel("Competence")
#plt.xlim()
plt.ylim(-0.1, 1.2)
plt.show()