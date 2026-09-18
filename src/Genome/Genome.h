#ifndef DHM_GENOME_GENOME_H
#define DHM_GENOME_GENOME_H

#include "Gene.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include <span>
#include <random>

namespace dhm::genome {

/**
 * @brief Represents a single chromosome containing an ordered sequence of genes.
 */
class Chromosome {
public:
    using ChromosomeId = uint32_t;

    explicit Chromosome(ChromosomeId id, std::string label = "")
        : id_(id), label_(std::move(label)) {}

    [[nodiscard]] constexpr ChromosomeId get_id() const noexcept { return id_; }
    [[nodiscard]] std::string_view get_label() const noexcept { return label_; }
    [[nodiscard]] std::span<const Gene> get_genes() const noexcept { return genes_; }
    [[nodiscard]] size_t size() const noexcept { return genes_.size(); }

    void add_gene(Gene gene) {
        genes_.push_back(std::move(gene));
    }

    [[nodiscard]] const Gene* find_gene(Gene::GeneId id) const noexcept {
        for (const auto& gene : genes_) {
            if (gene.get_id() == id) {
                return &gene;
            }
        }
        return nullptr;
    }

    void mutate(std::mt19937_64& rng, double intensity_multiplier = 1.0) {
        for (auto& gene : genes_) {
            gene.mutate(rng, intensity_multiplier);
        }
    }

private:
    ChromosomeId id_;
    std::string label_;
    std::vector<Gene> genes_;
};

/**
 * @brief Master genetic blueprint containing all chromosomes and driving phenotype expression.
 */
class Genome {
public:
    using GenomeId = uint64_t;

    explicit Genome(GenomeId id, std::string lineage_name = "Genesis")
        : id_(id), lineage_name_(std::move(lineage_name)) {}

    // Identification
    [[nodiscard]] constexpr GenomeId get_id() const noexcept { return id_; }
    [[nodiscard]] std::string_view get_lineage_name() const noexcept { return lineage_name_; }
    [[nodiscard]] std::span<const Chromosome> get_chromosomes() const noexcept { return chromosomes_; }

    // Construction & Mutation
    void add_chromosome(Chromosome chromosome);
    void mutate(std::mt19937_64& rng, double global_mutation_rate = 1.0);

    // Queries
    [[nodiscard]] const Gene* find_gene(Gene::GeneId gene_id) const noexcept;
    [[nodiscard]] const Gene* find_gene(std::string_view gene_name) const noexcept;
    
    // Phenotypic Extraction
    /**
     * @brief Extracts all active expressed parameter values for a target subsystem at a life stage.
     * @return Map of parameter names to their calculated float values.
     */
    [[nodiscard]] std::unordered_map<std::string, double> express_subsystem(
        SubsystemType target_subsystem, 
        DevelopmentalStage stage) const;

    /**
     * @brief Evaluates an individual gene parameter value, returning std::nullopt if missing.
     */
    [[nodiscard]] std::optional<double> express_parameter(
        std::string_view parameter_name, 
        DevelopmentalStage stage) const;

    void rebuild_index() noexcept;

private:
    GenomeId id_;
    std::string lineage_name_;
    std::vector<Chromosome> chromosomes_;
    std::unordered_map<std::string, const Gene*> parameter_to_gene_index_;

    
};

} // namespace dhm::genome

#endif // DHM_GENOME_GENOME_H
