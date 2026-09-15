#include "Genome.h"

namespace dhm::genome {

void Genome::add_chromosome(Chromosome chromosome) {
    chromosomes_.push_back(std::move(chromosome));
    rebuild_index();
}

void Genome::mutate(std::mt19937_64& rng, double global_mutation_rate) {
    for (auto& chromosome : chromosomes_) {
        chromosome.mutate(rng, global_mutation_rate);
    }
}

void Genome::rebuild_index() noexcept {
    parameter_to_gene_index_.clear();
    for (const auto& chromosome : chromosomes_) {
        for (const auto& gene : chromosome.get_genes()) {
            parameter_to_gene_index_[std::string(gene.get_parameter_target())] = &gene;
        }
    }
}

const Gene* Genome::find_gene(Gene::GeneId gene_id) const noexcept {
    for (const auto& chromosome : chromosomes_) {
        if (const auto* gene = chromosome.find_gene(gene_id); gene != nullptr) {
            return gene;
        }
    }
    return nullptr;
}

const Gene* Genome::find_gene(std::string_view gene_name) const noexcept {
    for (const auto& chromosome : chromosomes_) {
        for (const auto& gene : chromosome.get_genes()) {
            if (gene.get_name() == gene_name) {
                return &gene;
            }
        }
    }
    return nullptr;
}

std::unordered_map<std::string, double> Genome::express_subsystem(
    SubsystemType target_subsystem, 
    DevelopmentalStage stage) const {
    
    std::unordered_map<std::string, double> expressed_parameters;

    for (const auto& chromosome : chromosomes_) {
        for (const auto& gene : chromosome.get_genes()) {
            if (gene.get_target_subsystem() == target_subsystem) {
                expressed_parameters[std::string(gene.get_parameter_target())] = gene.express(stage);
            }
        }
    }

    return expressed_parameters;
}

std::optional<double> Genome::express_parameter(
    std::string_view parameter_name, 
    DevelopmentalStage stage) const {
    
    auto it = parameter_to_gene_index_.find(std::string(parameter_name));
    if (it != parameter_to_gene_index_.end() && it->second != nullptr) {
        return it->second->express(stage);
    }
    return std::nullopt;
}

} // namespace dhm::genome
