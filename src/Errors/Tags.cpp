#include "Errors/Tags.hpp"
#include "Logging.hpp"

namespace Errors::Tags {

void Exists::log() const {
	Logging::critical("Tag '{}' already exists", tag_name);
}
void DoesNotExist::log() const {
	Logging::critical("Tag '{}' does not exist", tag_name);
}

}  // namespace Errors::Tags
