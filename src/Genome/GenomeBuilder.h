#ifndef DHM_GENOME_GENOME_BUILDER_H
#define DHM_GENOME_GENOME_BUILDER_H

#include "Genome.h"

namespace dhm::genome {

class GenomeBuilder {
public:
    explicit GenomeBuilder(Genome::GenomeId id, std::string lineage);

    GenomeBuilder& new_chromosome(Chromosome::ChromosomeId id, std::string label);
    GenomeBuilder& add_gene(Gene gene);
    
    [[nodiscard]] Genome build();

    // Factory method for default wild-type genome
    [[nodiscard]] static Genome create_default_human_template();

private:
    Genome genome_;
    std::optional<Chromosome> current_chromosome_;
};

} // namespace dhm::genome

#endif // DHM_GENOME_GENOME_BUILDER_H
