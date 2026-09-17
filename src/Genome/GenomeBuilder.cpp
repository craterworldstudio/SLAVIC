#include "GenomeLoader.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

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

static DevelopmentalStage parse_stage(std::string_view str) noexcept {
    if (str == "Embryo")      return DevelopmentalStage::Embryo;
    if (str == "Infancy")     return DevelopmentalStage::Infancy;
    if (str == "Childhood")   return DevelopmentalStage::Childhood;
    if (str == "Adolescence") return DevelopmentalStage::Adolescence;
    return DevelopmentalStage::Adulthood;
}

std::expected<Genome, LoaderError> GenomeLoader::parse(std::string_view json_str) {
    try {
        json root = json::parse(json_str);

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

    } catch (const std::exception& e) {
        std::cerr << "[GenomeLoader Error] JSON Parse failed: " << e.what() << std::endl;
        return std::unexpected(LoaderError::MalformedJson);
    }
}

} // namespace dhm::genome
