#include "GenomeLoader.h"
#include <fstream>
#include <sstream>
#include <charconv>
#include <nlohmann/json.hpp>

namespace dhm::genome {

// Helper function to map JSON strings to DevelopmentalStage enums
static DevelopmentalStage parse_stage(std::string_view str) noexcept {
    if (str == "Embryo")      return DevelopmentalStage::Embryo;
    if (str == "Infancy")     return DevelopmentalStage::Infancy;
    if (str == "Childhood")   return DevelopmentalStage::Childhood;
    if (str == "Adolescence") return DevelopmentalStage::Adolescence;
    return DevelopmentalStage::Adulthood;
}

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
) {
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
) {
    if (raw_content.empty()) {
        return std::unexpected(LoaderError::CorruptData);
    }

    try {
        nlohmann::json root = nlohmann::json::parse(raw_content);

        uint64_t genome_id = root.value("genome_id", 1ULL);
        std::string lineage = root.value("lineage", "Unknown-Lineage");

        Genome genome(genome_id, lineage);

        if (root.contains("chromosomes") && root["chromosomes"].is_array()) {
            for (const auto& chrom_json : root["chromosomes"]) {
                uint32_t chrom_id = chrom_json.value("id", 1U);
                std::string label = chrom_json.value("label", "Chromosome");
                Chromosome chromosome(chrom_id, label);

                if (chrom_json.contains("genes") && chrom_json["genes"].is_array()) {
                    for (const auto& gene_json : chrom_json["genes"]) {
                        Allele maternal{
                            gene_json["maternal_allele"].value("value", 0.0),
                            gene_json["maternal_allele"].value("dominance", 1.0),
                            gene_json["maternal_allele"].value("is_active", true)
                        };

                        Allele paternal{
                            gene_json["paternal_allele"].value("value", 0.0),
                            gene_json["paternal_allele"].value("dominance", 1.0),
                            gene_json["paternal_allele"].value("is_active", true)
                        };

                        ExpressionWindow window{
                            parse_stage(gene_json["window"].value("onset", "Embryo")),
                            parse_stage(gene_json["window"].value("peak", "Embryo")),
                            parse_stage(gene_json["window"].value("decay", "Adulthood"))
                        };

                        Gene gene(
                            gene_json.value("id", 0ULL),
                            gene_json.value("name", "UnnamedGene"),
                            parse_subsystem(gene_json.value("subsystem", "Hormones")),
                            gene_json.value("target", "unassigned_target"),
                            maternal,
                            paternal,
                            window,
                            gene_json.value("mutation_rate", 0.01),
                            gene_json.value("is_mutable", true)
                        );

                        chromosome.add_gene(std::move(gene));
                    }
                }
                genome.add_chromosome(std::move(chromosome));
            }
        }

        genome.rebuild_index(); 
        return genome;

    } catch (const nlohmann::json::parse_error&) {
        return std::unexpected(LoaderError::MalformedJson);
    } catch (const nlohmann::json::exception&) {
        return std::unexpected(LoaderError::ParseError);
    }
}

} // namespace dhm::genome