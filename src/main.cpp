#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Genome/GenomeLoader.h"
#include "Brain/Brain.h"

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  --genome <path/to/genome.json>   Load a specific digital genome file.\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << " S.L.A.V.I.C (DHM-02) - Core Runtime                    " << std::endl;
    std::cout << "==========================================" << std::endl;

    // Default fallback if no flag is provided
    std::string genome_path = "assets/genomes/adam.json";

    // Basic CLI Argument Parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--genome" && i + 1 < argc) {
            genome_path = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
    }

    std::cout << "[Boot] Loading genome from: " << genome_path << std::endl;
    std::ifstream file(genome_path);

    if (!file.is_open()) {
        std::cerr << "[Fatal Error] Could not open genome file: " << genome_path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto genome_result = dhm::genome::GenomeLoader::parse(buffer.str());
    if (!genome_result) {
        std::cerr << "[Fatal Error] Genome JSON failed to parse." << std::endl;
        return 1;
    }

    std::cout << "[Genome] Successfully loaded lineage: " << genome_result->get_lineage_name() << std::endl;

    try {
        dhm::brain::Brain brain;
        brain.bootstrap(*genome_result);

        std::cout << "[Simulation] Running embryonic ticks on GPU..." << std::endl;
        for (int i = 0; i < 10; ++i) {
            brain.tick(0.5f);
        }
        std::cout << "[Simulation] Ticks completed successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
