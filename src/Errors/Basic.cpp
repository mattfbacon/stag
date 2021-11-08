#include "Errors/Basic.hpp"

#include "common.hpp"
#include "info.hpp"

namespace Errors::Basic {

void User::print_to(std::ostream& os) const {
	os << STAG_BINARY_NAME ": " LOGLEVEL_USER ": " << message << std::endl;
}
void Fatal::print_to(std::ostream& os) const {
	os << STAG_BINARY_NAME ": " LOGLEVEL_FATAL ": " << message << std::endl;
	print_usage_to(os);
}

}  // namespace Errors::Basic
