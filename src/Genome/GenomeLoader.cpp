#ifndef DHM_GENOME_GENOME_LOADER_H
#define DHM_GENOME_GENOME_LOADER_H

#include "Genome.h"
#include <filesystem>
#include <expected>
#include <string>

namespace dhm::genome {

enum class LoaderError : uint8_t {
    FileNotFound,
    ReadFailed,
    ParseError,
    CorruptData,
    InvalidSchema
};

class GenomeLoader {
public:
    /**
     * @brief Loads and parses a digital genome definition from disk.
     */
    [[nodiscard]] static std::expected<Genome, LoaderError> load_from_file(
        const std::filesystem::path& file_path
    ) noexcept;

    /**
     * @brief Direct text buffer parsing.
     */
    [[nodiscard]] static std::expected<Genome, LoaderError> parse(
        std::string_view raw_content
    ) noexcept;

private:
    [[nodiscard]] static SubsystemType parse_subsystem(std::string_view str) noexcept;
};

} // namespace dhm::genome

#endif // DHM_GENOME_GENOME_LOADER_H
