#ifndef DHM_GENOME_GENOME_VALIDATOR_H
#define DHM_GENOME_GENOME_VALIDATOR_H

#include "Genome.h"
#include <vector>
#include <string>

namespace dhm::genome {

struct ValidationIssue {
    enum class Severity { Warning, Critical };
    Severity severity;
    std::string message;
};

class GenomeValidator {
public:
    [[nodiscard]] static std::vector<ValidationIssue> validate(const Genome& genome) noexcept;
    [[nodiscard]] static bool is_viable(const Genome& genome) noexcept;
};

} // namespace dhm::genome

#endif // DHM_GENOME_GENOME_VALIDATOR_H
