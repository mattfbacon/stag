#include <cassert>

#include "Errors/Abbreviation.hpp"
#include "Filesystem.hpp"
#include "Logging.hpp"
#include "Logic/AllDirectory.hpp"

namespace Logic::AllDirectory {

size_t num_files() {
	size_t ret = 0;
	for (auto const& dirent : std::filesystem::directory_iterator{ Filesystem::path_all }) {
		if (!dirent.is_regular_file()) {
			continue;
		}
		++ret;
	}
	return ret;
}

SplitAbbreviation split_abbreviation(std::string_view abbreviation) {
	auto const path = std::filesystem::path{ abbreviation };
	return { path.stem().string(), path.extension().string() };
}

bool abbreviation_matches(SplitAbbreviation const& split_abbr, std::filesystem::path const& path) {
	auto const& [abbr_hash, abbr_extension] = split_abbr;
	return path.stem().string().starts_with(abbr_hash) && path.extension().string().starts_with(abbr_extension);
}

// This algorithm prioritizes user-friendliness over speed.
// If speed were desired, the central loop could quit early when a match is found.
// FIXME: could this be optimized if the caller gave a list of abbreviations?
std::filesystem::path expand_file_abbreviation(std::string_view const abbreviation) {
	Logging::trace("Expanding file abbreviation '{}'", abbreviation);
	auto const split_abbr = split_abbreviation(abbreviation);

	Filesystem::FilesSet expansions;
	for (auto const& dirent : std::filesystem::directory_iterator{ Filesystem::path_all }) {
		if (!dirent.is_regular_file()) {
			Logging::trace("ExpandAbbr({}): skipping non-file '{}'", abbreviation, dirent.path());
			continue;
		}
		if (abbreviation_matches(split_abbr, dirent.path().filename())) {
			Logging::debug("ExpandAbbr({}): found possible expansion '{}'", abbreviation, dirent.path());
			expansions.insert(dirent);
		}
	}
	switch (expansions.size()) {
		case 1:
			return *expansions.begin();
		case 0:
			throw Errors::Abbreviation::NoMatch{ std::string{ abbreviation } };
		default:
			throw Errors::Abbreviation::Ambiguous{ std::string{ abbreviation }, std::move(expansions) };
	}
}

}  // namespace Logic::AllDirectory
