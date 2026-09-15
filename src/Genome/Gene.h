#ifndef SLAVIC_GENETICS_GENE_H
#define SLAVIC_GENETICS_GENE_H

#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>

namespace slavic::genetics {

/**
 * @brief Subsystems influenced by gene expression.
 */
enum class SubsystemType : uint8_t {
    BrainTopology,    // Initial node counts, growth density bounds
    Plasticity,       // STDP rates, synaptogenesis thresholds, pruning rates
    Drives,           // Base homeostatic targets, curiosity bias, decay rates
    Hormones,         // Baseline dopamine/noradrenaline levels, receptor density
    Perception,       // Sensory resolution, attentional filters
    Motor,            // Primitive motor babbling frequency, muscular precision
    Metacognition     // Self-monitoring threshold, prediction error sensitivity
};

/**
 * @brief Life cycles during which a gene can trigger or modulate expression.
 */
enum class DevelopmentalStage : uint8_t {
    Embryo = 0,
    Infancy,
    Childhood,
    Adolescence,
    Adulthood
};

/**
 * @brief Represents an Allele variant carrying a quantitative value and dominance metadata.
 */
struct Allele {
    double value{0.0};
    double dominance{1.0}; // 1.0 = Fully Dominant, 0.0 = Fully Recessive
    bool is_active{true};
};

/**
 * @brief Defines the activation window for a gene across developmental timelines.
 */
struct ExpressionWindow {
    DevelopmentalStage onset_stage{DevelopmentalStage::Embryo};
    DevelopmentalStage peak_stage{DevelopmentalStage::Embryo};
    DevelopmentalStage decay_stage{DevelopmentalStage::Adulthood};
    
    // Scale factor (0.0 to 1.0) calculated based on current stage tick
    [[nodiscard]] constexpr double get_expression_factor(DevelopmentalStage current_stage) const noexcept {
        if (current_stage < onset_stage || current_stage > decay_stage) {
            return 0.0;
        }
        if (current_stage == peak_stage) {
            return 1.0;
        }
        // Linear scaling fallback between onset and peak / decay
        return 0.5; 
    }
};

/**
 * @brief Unit of digital inheritance encoding developmental rules for S.L.A.V.I.C.
 */
class Gene {
public:
    using GeneId = uint64_t;

    Gene(GeneId id,
         std::string name,
         SubsystemType target_subsystem,
         std::string parameter_target,
         Allele maternal_allele,
         Allele paternal_allele,
         ExpressionWindow window,
         double mutation_rate = 0.01,
         bool is_mutable = true)
        : id_(id),
          name_(std::move(name)),
          target_subsystem_(target_subsystem),
          parameter_target_(std::move(parameter_target)),
          maternal_allele_(maternal_allele),
          paternal_allele_(paternal_allele),
          expression_window_(window),
          mutation_rate_(mutation_rate),
          is_mutable_(is_mutable) {}

    // --- Identification & Metadata ---
    [[nodiscard]] constexpr GeneId get_id() const noexcept { return id_; }
    [[nodiscard]] std::string_view get_name() const noexcept { return name_; }
    [[nodiscard]] constexpr SubsystemType get_target_subsystem() const noexcept { return target_subsystem_; }
    [[nodiscard]] std::string_view get_parameter_target() const noexcept { return parameter_target_; }

    // --- Alleles & Dominance ---
    [[nodiscard]] const Allele& get_maternal_allele() const noexcept { return maternal_allele_; }
    [[nodiscard]] const Allele& get_paternal_allele() const noexcept { return paternal_allele_; }

    // --- Expression Logic ---
    /**
     * @brief Computes the expressed phenotype value modulated by current developmental stage.
     */
    [[nodiscard]] double express(DevelopmentalStage current_stage) const noexcept {
        const double window_factor = expression_window_.get_expression_factor(current_stage);
        if (window_factor <= 0.0) {
            return 0.0;
        }

        // Dominance-weighted blend of maternal and paternal alleles
        const double total_dominance = maternal_allele_.dominance + paternal_allele_.dominance;
        double base_expressed_value = 0.0;

        if (total_dominance > 0.0) {
            base_expressed_value = ((maternal_allele_.value * maternal_allele_.dominance) +
                                    (paternal_allele_.value * paternal_allele_.dominance)) / total_dominance;
        } else {
            base_expressed_value = (maternal_allele_.value + paternal_allele_.value) * 0.5;
        }

        return base_expressed_value * window_factor;
    }

    // --- Mutation Logic ---
    /**
     * @brief Mutates alleles if mutable and within mutation probability bounds.
     */
    void mutate(std::mt19937_64& rng, double intensity_multiplier = 1.0) {
        if (!is_mutable_) return;

        std::uniform_real_distribution<double> dist(0.0, 1.0);
        std::normal_distribution<double> variance(0.0, 0.05 * intensity_multiplier);

        if (dist(rng) < mutation_rate_) {
            maternal_allele_.value += variance(rng);
        }
        if (dist(rng) < mutation_rate_) {
            paternal_allele_.value += variance(rng);
        }
    }

    // --- Mutability Flags ---
    [[nodiscard]] constexpr bool is_mutable() const noexcept { return is_mutable_; }
    constexpr void set_mutable(bool state) noexcept { is_mutable_ = state; }

private:
    GeneId id_;
    std::string name_;
    SubsystemType target_subsystem_;
    std::string parameter_target_; // e.g., "dopamine_reuptake_rate", "max_synapse_density"
    
    Allele maternal_allele_;
    Allele paternal_allele_;
    
    ExpressionWindow expression_window_;
    double mutation_rate_{0.01};
    bool is_mutable_{true};
};

} // namespace slavic::genetics

#endif // SLAVIC_GENETICS_GENE_H
