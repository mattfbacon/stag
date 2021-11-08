#include "Errors/Abbreviation.hpp"

namespace Errors::Abbreviation {

void NoMatch::print_to(std::ostream& os) const {
	os << "Abbreviation '" << abbreviation << "' had no matches." << std::endl;
}

void Ambiguous::print_to(std::ostream& os) const {
	os << "Abbreviation '" << abbreviation << "' was ambiguous. Expansion candidates:\n";
	for (auto const& candidate : expansions) {
		os << "  " << candidate << '\n';
	}
	os << "Note that the candidates list may not be complete" << std::endl;
}

}  // namespace Errors::Abbreviation
