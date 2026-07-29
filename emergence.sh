#!/usr/bin/env bash

# ==========================================================
# Digital Human Mind (DHM)
# Project Structure Generator
# ==========================================================

PROJECT_NAME="DHM"

# ----------------------------------------------------------
# Root
# ----------------------------------------------------------

touch \
    CMakeLists.txt \
    README.md \
    LICENSE

mkdir -p \
    assets \
    docs \
    examples \
    logs \
    tests \
    tools \
    genomes \
    cmake \
    include \
    src

# ----------------------------------------------------------
# Brain
# ----------------------------------------------------------

mkdir -p src/Brain

touch \
    src/Brain/Brain.h \
    src/Brain/Brain.cpp \
    src/Brain/BrainBootstrap.cpp \
    src/Brain/BrainClock.cpp \
    src/Brain/BrainShutdown.cpp \
    src/Brain/BrainState.h \
    src/Brain/BrainState.cpp

# ----------------------------------------------------------
# Genome
# ----------------------------------------------------------

mkdir -p src/Genome

touch \
    src/Genome/Genome.h \
    src/Genome/Genome.cpp \
    src/Genome/Gene.h \
    src/Genome/Gene.cpp \
    src/Genome/GenomeLoader.cpp \
    src/Genome/GenomeBuilder.cpp \
    src/Genome/GenomeValidator.cpp

# ----------------------------------------------------------
# Development
# ----------------------------------------------------------

mkdir -p src/Development

touch \
    src/Development/DevelopmentManager.h \
    src/Development/DevelopmentManager.cpp \
    src/Development/Embryo.cpp \
    src/Development/Infant.cpp \
    src/Development/Child.cpp \
    src/Development/Teen.cpp \
    src/Development/Adult.cpp \
    src/Development/Aging.cpp

# ----------------------------------------------------------
# Memory
# ----------------------------------------------------------

mkdir -p \
    src/Memory/WorkingMemory \
    src/Memory/EpisodicMemory \
    src/Memory/SemanticMemory \
    src/Memory/ProceduralMemory \
    src/Memory/Autobiography \
    src/Memory/DreamMemory

touch \
    src/Memory/MemoryEngine.h \
    src/Memory/MemoryEngine.cpp

# ----------------------------------------------------------
# Emotion
# ----------------------------------------------------------

mkdir -p src/Emotion

touch \
    src/Emotion/EmotionEngine.h \
    src/Emotion/EmotionEngine.cpp \
    src/Emotion/Hormones.cpp \
    src/Emotion/Mood.cpp \
    src/Emotion/Stress.cpp \
    src/Emotion/Pain.cpp \
    src/Emotion/Reward.cpp

# ----------------------------------------------------------
# Drives
# ----------------------------------------------------------

mkdir -p \
    src/Drive/Needs \
    src/Drive/Instincts \
    src/Drive/Goals \
    src/Drive/Conflict

touch \
    src/Drive/DriveEngine.h \
    src/Drive/DriveEngine.cpp

# ----------------------------------------------------------
# Perception
# ----------------------------------------------------------

mkdir -p \
    src/Perception/Vision \
    src/Perception/Audio \
    src/Perception/Touch \
    src/Perception/Proprioception \
    src/Perception/InternalSense

touch \
    src/Perception/PerceptionEngine.h \
    src/Perception/PerceptionEngine.cpp

# ----------------------------------------------------------
# Prediction
# ----------------------------------------------------------

mkdir -p \
    src/Prediction/WorldModel \
    src/Prediction/SelfModel \
    src/Prediction/FutureSimulator \
    src/Prediction/ErrorCalculator

touch \
    src/Prediction/PredictionEngine.h \
    src/Prediction/PredictionEngine.cpp

# ----------------------------------------------------------
# Learning
# ----------------------------------------------------------

mkdir -p \
    src/Learning/Associations \
    src/Learning/Reinforcement \
    src/Learning/Habits \
    src/Learning/Concepts \
    src/Learning/Generalization

touch \
    src/Learning/LearningEngine.h \
    src/Learning/LearningEngine.cpp

# ----------------------------------------------------------
# Decision
# ----------------------------------------------------------

mkdir -p \
    src/Decision/Attention \
    src/Decision/Planner \
    src/Decision/ActionSelection \
    src/Decision/Execution

touch \
    src/Decision/DecisionEngine.h \
    src/Decision/DecisionEngine.cpp

# ----------------------------------------------------------
# Language
# ----------------------------------------------------------

mkdir -p \
    src/Language/ConceptGraph \
    src/Language/Parser \
    src/Language/Speech \
    src/Language/Reading \
    src/Language/Writing

touch \
    src/Language/LanguageEngine.h \
    src/Language/LanguageEngine.cpp

# ----------------------------------------------------------
# Motor
# ----------------------------------------------------------

mkdir -p src/Motor

touch \
    src/Motor/MotorSystem.h \
    src/Motor/MotorSystem.cpp

# ----------------------------------------------------------
# Body
# ----------------------------------------------------------

mkdir -p \
    src/Body/Skeleton \
    src/Body/Muscles \
    src/Body/Energy \
    src/Body/Health \
    src/Body/Metabolism

touch \
    src/Body/Body.h \
    src/Body/Body.cpp

# ----------------------------------------------------------
# Simulation
# ----------------------------------------------------------

mkdir -p src/Simulation

touch \
    src/Simulation/Simulation.h \
    src/Simulation/Simulation.cpp

# ----------------------------------------------------------
# Serialization
# ----------------------------------------------------------

mkdir -p src/Serialization

touch \
    src/Serialization/Serializer.h \
    src/Serialization/Serializer.cpp

# ----------------------------------------------------------
# Utilities
# ----------------------------------------------------------

mkdir -p src/Utilities

touch \
    src/Utilities/Logger.h \
    src/Utilities/Logger.cpp \
    src/Utilities/Timer.h \
    src/Utilities/Timer.cpp \
    src/Utilities/Random.h \
    src/Utilities/Random.cpp

# ----------------------------------------------------------
# Main
# ----------------------------------------------------------

touch \
    src/main.cpp

echo
echo "=========================================="
echo " Digital Human Mind project created."
echo "=========================================="