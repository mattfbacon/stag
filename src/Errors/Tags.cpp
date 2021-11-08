#include "Errors/Tags.hpp"

namespace Errors::Tags {

void Exists::print_to(std::ostream& os) const {
	os << "Tag already exists: " << tag_name << std::endl;
}
void DoesNotExist::print_to(std::ostream& os) const {
	os << "Tag does not exist: " << tag_name << std::endl;
}

}  // namespace Errors::Tags
