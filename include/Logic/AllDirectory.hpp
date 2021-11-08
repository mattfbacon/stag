#pragma once

#include <filesystem>
#include <string>
#include <tuple>

namespace Logic::AllDirectory {

size_t num_files();

using SplitAbbreviation = std::tuple<std::string, std::string>;

SplitAbbreviation split_abbreviation(std::string_view abbreviation);

bool abbreviation_matches(SplitAbbreviation const& split_abbr, std::filesystem::path const& path);
inline bool abbreviation_matches(std::string_view abbreviation, std::filesystem::path const& path) {
	return abbreviation_matches(split_abbreviation(abbreviation), path);
}

std::filesystem::path expand_file_abbreviation(std::string_view abbreviation);

}  // namespace Logic::AllDirectory
