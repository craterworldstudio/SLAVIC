#include "GenomeValidator.h"

namespace dhm::genome {

std::vector<ValidationIssue> GenomeValidator::validate(const Genome& genome) noexcept {
    std::vector<ValidationIssue> issues;

    if (genome.get_chromosomes().empty()) {
        issues.push_back({ValidationIssue::Severity::Critical, "Genome contains no chromosomes."});
        return issues;
    }

    // Ensure fundamental autonomic/neuromodulatory genes exist
    bool has_dopamine = false;
    bool has_drives = false;

    for (const auto& chrom : genome.get_chromosomes()) {
        for (const auto& gene : chrom.get_genes()) {
            if (gene.get_target_subsystem() == SubsystemType::Hormones) {
                has_dopamine = true;
            }
            if (gene.get_target_subsystem() == SubsystemType::Drives) {
                has_drives = true;
            }

            // Allele range sanity checks
            if (gene.get_maternal_allele().dominance < 0.0 || gene.get_paternal_allele().dominance < 0.0) {
                issues.push_back({ValidationIssue::Severity::Critical, 
                    "Negative dominance factor detected on gene: " + std::string(gene.get_name())});
            }
        }
    }

    if (!has_dopamine) {
        issues.push_back({ValidationIssue::Severity::Critical, "Missing required Hormones subsystem genes."});
    }
    if (!has_drives) {
        issues.push_back({ValidationIssue::Severity::Warning, "Genome lacks initial homeostatic drives."});
    }

    return issues;
}

bool GenomeValidator::is_viable(const Genome& genome) noexcept {
    auto issues = validate(genome);
    for (const auto& issue : issues) {
        if (issue.severity == ValidationIssue::Severity::Critical) {
            return false;
        }
    }
    return true;
}

} // namespace dhm::genome
