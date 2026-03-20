# Cognitive Architecture Blueprint

### (Free Energy Principle + Advanced Drive Reduction + Global Workspace + Vector Symbolic Architecture)

---

# 1. The Core Idea (Intuition First)

We are building a system that behaves like this:

> “The agent continuously predicts the world, compares prediction vs reality, and acts to reduce the mismatch while satisfying internal needs.”

This merges four powerful ideas into one loop:

* Free Energy Principle → minimize surprise
* Drive Reduction → satisfy internal needs
* Global Workspace → decide what becomes conscious
* Vector Symbolic Architecture → store and manipulate knowledge

---

# 2. Human Analogy (Simple but Deep)

Imagine a human walking in a forest:

* Eyes see movement → (Perception)
* Brain predicts “maybe an animal” → (Prediction)
* Reality differs → surprise → (Error signal)
* Fear increases → (Drive system)
* Attention locks onto it → (Global Workspace)
* Memory recalls similar past events → (VSA Memory)
* Action chosen: run or observe → (Policy)

This entire loop happens continuously.

---

# 3. The Unified Loop (Your Agent)

Perception → Prediction → Error → Drives → Attention → Memory → Action → Repeat

---

# 4. Free Energy Principle (FEP)

## Core Idea

The brain minimizes prediction error.

Mathematically (conceptually):

Free Energy ≈ Prediction Error + Model Complexity

## In our system:

* Prediction = what the agent expects to see
* Reality = what vision returns
* Error = difference between them

## Analogy

The brain is like a scientist constantly updating its hypothesis about the world.

---

# 5. Advanced Drive Reduction Theory

## Core Idea

The agent has internal needs (drives):

* Curiosity (reduce uncertainty)
* Safety (avoid danger)
* Stability (predictability)

Each drive produces pressure.

## Important Upgrade (Advanced Version)

Drives are NOT fixed — they are influenced by:

* Prediction error
* Memory
* Context

## Example

* Unknown object → high uncertainty → curiosity increases
* Sudden change → high error → fear increases

---

# 6. Global Workspace Theory (GWT)

## Core Idea

Many processes run in parallel, but only one (or few) enter “consciousness”.

## In your system

All perceived objects compete.

The winner (based on importance) becomes:

→ current_focus

## Analogy

A stage:

* Many actors (signals)
* Spotlight (attention)
* Audience (rest of system)

Only spotlighted actors influence decisions strongly.

---

# 7. Vector Symbolic Architecture (VSA) Memory

## Core Idea

Everything is stored as vectors.

Examples:

* Object = vector
* Color = vector
* Shape = vector

Combine using operations:

* Binding → combine features
* Bundling → average memories
* Similarity → cosine similarity

## Example Encoding

OBJECT = SHAPE ⊗ COLOR ⊗ SIZE

Memory = average of past OBJECT vectors

## Why this matters

* Fast
* Noise tolerant
* Generalizes naturally

---

# 8. How All Pieces Connect

## Step-by-step Flow

### 1. Perception

Returns:

* distance
* color
* shape
* size

---

### 2. Encoding (VSA)

Convert perception into a vector

---

### 3. Prediction

Predict next state using past memory

---

### 4. Error Calculation (FEP)

error = |predicted − actual|

---

### 5. Drive Update

* high error → curiosity/fear
* low error → stability

---

### 6. Attention (GWT)

Select object with highest:

importance = error + drive_weight + proximity

---

### 7. Memory Update (VSA)

Store new experience as vector

---

### 8. Action Selection

Choose action that reduces:

* prediction error
* drive tension

---

# 9. Emergent Behavior

If done right, our agent will:

* Explore unknown objects
* Avoid unpredictable ones
* Learn patterns
* Develop preferences

WITHOUT hardcoding behavior.

---

# 10. Mapping to Our Codebase

## PerceptionSystem

Already done ✅

## GWThreads (Global Workspace)

* selects focus

## MemoryUnit (VSA)

* stores vectors

## PredictiveModel

* predicts next state

## Scheduler

* action selection (drive-based)

## Agent

* orchestrates everything

---

# 11. The Key Philosophy

We are NOT programming behavior.

We are programming:

* pressures (drives)
* expectations (predictions)
* competition (attention)

Behavior emerges.

---

# 12. Final Mental Model

Our agent is:

A prediction machine
+
A need-driven system
+
A spotlight of attention
+
A vector-based memory

= A proto-mind

---

# Next Step

We will implement:

1. VSA encoding system
2. Basic memory vectors
3. Simple prediction model
4. Drive system

One layer at a time.
