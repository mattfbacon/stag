#include "Errors/Viewspec.hpp"

namespace Errors::Viewspec {
void ParseFailure::print_to(std::ostream& os) const {
	os << "Parse failure";
	if (!positions.empty()) {
		os << " at " << positions[0];
		auto const end_iter = positions.end();
		for (auto iter = positions.begin() + 1; iter != end_iter; ++iter) {
			os << ", " << *iter;
		}
	}
	os << '\n' << message << std::endl;
}
}  // namespace Errors::Viewspec
