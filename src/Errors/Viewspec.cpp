#include "Errors/Viewspec.hpp"
#include "Logging.hpp"

namespace Errors::Viewspec {

void ParseFailure::log() const {
	Logging::critical("Parse failure{}{}", positions.empty() ? "" : " at ", fmt::join(positions, ", "));
}

}  // namespace Errors::Viewspec
