#include "GenomeBuilder.h"

namespace dhm::genome {

GenomeBuilder::GenomeBuilder(Genome::GenomeId id, std::string lineage)
    : genome_(id, std::move(lineage)) {}

GenomeBuilder& GenomeBuilder::new_chromosome(Chromosome::ChromosomeId id, std::string label) {
    if (current_chromosome_.has_value()) {
        genome_.add_chromosome(std::move(*current_chromosome_));
    }
    current_chromosome_ = Chromosome(id, std::move(label));
    return *this;
}

GenomeBuilder& GenomeBuilder::add_gene(Gene gene) {
    if (current_chromosome_.has_value()) {
        current_chromosome_->add_gene(std::move(gene));
    }
    return *this;
}

Genome GenomeBuilder::build() {
    if (current_chromosome_.has_value()) {
        genome_.add_chromosome(std::move(*current_chromosome_));
        current_chromosome_.reset();
    }
    return std::move(genome_);
}

Genome GenomeBuilder::create_default_human_template() {
    GenomeBuilder builder(1001, "Genesis-Baseline");

    // Chromosome 1: Neuromodulatory System
    builder.new_chromosome(1, "Autonomic-Neuromodulators")
        .add_gene(Gene{
            1, "dopamine_baseline_rate", SubsystemType::Hormones, "dopamine_clearance_rate",
            Allele{0.05, 1.0, true}, Allele{0.05, 1.0, true},
            ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Embryo, DevelopmentalStage::Adulthood},
            0.01, true
        })
        .add_gene(Gene{
            2, "noradrenaline_sensitivity", SubsystemType::Hormones, "noradrenaline_arousal_gain",
            Allele{0.80, 1.0, true}, Allele{0.75, 0.8, true},
            ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Infancy, DevelopmentalStage::Adulthood},
            0.02, true
        });

    // Chromosome 2: Neuroplastic Growth Rules
    builder.new_chromosome(2, "Morphology-Plasticity")
        .add_gene(Gene{
            3, "stdp_potentiation_rate", SubsystemType::Plasticity, "stdp_ltp_rate",
            Allele{0.015, 1.0, true}, Allele{0.012, 1.0, true},
            ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Childhood, DevelopmentalStage::Adulthood},
            0.01, true
        })
        .add_gene(Gene{
            4, "structural_synaptogenesis_threshold", SubsystemType::Plasticity, "synapse_spawn_threshold",
            Allele{0.70, 1.0, true}, Allele{0.65, 0.5, true},
            ExpressionWindow{DevelopmentalStage::Embryo, DevelopmentalStage::Infancy, DevelopmentalStage::Adolescence},
            0.02, true
        });

    return builder.build();
}

} // namespace dhm::genome
