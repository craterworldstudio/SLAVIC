#include "GenomeLoader.h"
#include <fstream>
#include <sstream>
#include <charconv>

namespace dhm::genome {

SubsystemType GenomeLoader::parse_subsystem(std::string_view str) noexcept {
    if (str == "BrainTopology") return SubsystemType::BrainTopology;
    if (str == "Plasticity")    return SubsystemType::Plasticity;
    if (str == "Drives")        return SubsystemType::Drives;
    if (str == "Hormones")      return SubsystemType::Hormones;
    if (str == "Perception")    return SubsystemType::Perception;
    if (str == "Motor")         return SubsystemType::Motor;
    return SubsystemType::Metacognition;
}

std::expected<Genome, LoaderError> GenomeLoader::load_from_file(
    const std::filesystem::path& file_path
) noexcept {
    if (!std::filesystem::exists(file_path)) {
        return std::unexpected(LoaderError::FileNotFound);
    }

    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return std::unexpected(LoaderError::ReadFailed);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return parse(ss.str());
}

std::expected<Genome, LoaderError> GenomeLoader::parse(
    std::string_view raw_content
) noexcept {
    if (raw_content.empty()) {
        return std::unexpected(LoaderError::CorruptData);
    }

    // Default embryonic initialization
    Genome genome(1001, "Genesis-Embryo");

    Chromosome primary_chromosome(1, "Autonomic-Core");

    // Pre-wired developmental curiosity drive
    Gene curiosity_gene(
        101,
        "curiosity_baseline",
        SubsystemType::Drives,
        "drive_curiosity_base",
        Allele{0.85, 1.0, true},
        Allele{0.70, 0.5, true},
        ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Infancy, DevelopmentalStage::Adulthood},
        0.02,
        true
    );

    // Pre-wired neurochemical baseline
    Gene dopamine_gene(
        102,
        "dopamine_clearance",
        SubsystemType::Hormones,
        "dopamine_clearance_rate",
        Allele{0.05, 0.8, true},
        Allele{0.04, 0.8, true},
        ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Embryo, DevelopmentalStage::Adulthood},
        0.01,
        true
    );

    primary_chromosome.add_gene(std::move(curiosity_gene));
    primary_chromosome.add_gene(std::move(dopamine_gene));
    genome.add_chromosome(std::move(primary_chromosome));

    return genome;
}

} // namespace dhm::genome
