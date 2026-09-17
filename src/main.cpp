#include <iostream>
#include "Genome/GenomeBuilder.h"
#include "Brain/Brain.h"

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << " Digital Human Mind (DHM) - Core Runtime  " << std::endl;
    std::cout << "==========================================" << std::endl;

    try {
        // 1. Build wild-type genome
        std::cout << "[Phase 0] Synthesizing Adam Genome Template..." << std::endl;
        auto genome = dhm::genome::GenomeBuilder::create_default_human_template();

        // 2. Initialize Spiking Brain with Vulkan Compute Backend
        std::cout << "[Phase 0] Instantiating Spiking Brain Context..." << std::endl;
        dhm::brain::Brain brain;

        // 3. Boot embryo from genetic code
        brain.bootstrap(genome);

        std::cout << "[Phase 0] System operational on dedicated GPU." << std::endl;

        std::cout << "[Simulation] Running 10 embryonic ticks on GPU..." << std::endl;
        for (int i = 0; i < 10; ++i) {
            brain.tick(0.5f); // 0.5 ms per step
        }
        std::cout << "[Simulation] Ticks completed successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
