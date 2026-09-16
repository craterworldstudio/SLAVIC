#include "Gene.h"

namespace slavic::genetics {

// Explicit out-of-line implementations if non-trivial logic expands
double compute_dominance_blend(const Allele& maternal, const Allele& paternal) noexcept {
    const double total_dominance = maternal.dominance + paternal.dominance;
    if (total_dominance > 0.0) {
        return ((maternal.value * maternal.dominance) +
                (paternal.value * paternal.dominance)) / total_dominance;
    }
    return (maternal.value + paternal.value) * 0.5;
}

} // namespace slavic::genetics
