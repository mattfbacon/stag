#include <algorithm>
#include <ranges>

#include "Errors/Abbreviation.hpp"
#include "Logging.hpp"

#include "spdlog/fmt/bundled/core.h"

namespace Errors::Abbreviation {

void NoMatch::log() const {
	// Logging::error("Abbreviation '{}' had no matches.", abbreviation);
}

void Ambiguous::log() const {
	Logging::error(
		"Abbreviation '{}' was ambiguous. Expansion candidates:\n  {}\n"
		"Note that the candidate list may not be complete.",
		abbreviation,
		fmt::join(expansions, "\n  "));
}

}  // namespace Errors::Abbreviation
