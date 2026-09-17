#!/usr/bin/env bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p src/Brain/Shaders/spv
glslc src/Brain/Shaders/neuron_step.comp -o src/Brain/Shaders/spv/neuron_step.spv

mkdir -p "${PROJECT_DIR}/build"
cd "${PROJECT_DIR}/build"

cmake "${PROJECT_DIR}"
cmake --build . -j"$(nproc)"

echo "Build complete: ./build/DHM-02"