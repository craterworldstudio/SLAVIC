#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Genome/GenomeLoader.h"
#include "Brain/Brain.h"

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << " S.L.A.V.I.C (DHM-02) - Core Runtime      " << std::endl;
    std::cout << "==========================================" << std::endl;

    std::string genome_path = "genomes/adam.json";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--genome" && i + 1 < argc) {
            genome_path = argv[++i];
        }
    }

    std::cout << "[Boot] Loading genome from: " << genome_path << std::endl;
    auto genome_result = dhm::genome::GenomeLoader::load_from_file(genome_path);
    if (!genome_result) {
        std::cerr << "[Fatal Error] Genome failed to load." << std::endl;
        return 1;
    }

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "[Fatal Error] Failed to initialize GLFW." << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell GLFW we are using Vulkan, not OpenGL
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "S.L.A.V.I.C. Environment", nullptr, nullptr);
    if (!window) {
        std::cerr << "[Fatal Error] Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return 1;
    }

    try {
        dhm::brain::Brain brain;
        brain.bootstrap(*genome_result);

        std::cout << "[Simulation] Entering main loop..." << std::endl;
        
        // Real-time application loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            
            // In the future, delta_time will be calculated here
            brain.tick(0.016f); // Simulate ~60fps brain ticks
            
            // TODO: Step Jolt Physics World
            // TODO: Render 3D Scene
        }

        std::cout << "[Brain] Shutting down simulation clock..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}