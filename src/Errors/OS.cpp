#include <magic_enum.hpp>

#include "Errors/Base.hpp"
#include "Errors/OS.hpp"

namespace Errors::OS {

void Permissions::print_to(std::ostream& os) const {
	os << STAG_BINARY_NAME ": " LOGLEVEL_FATAL ": Missing " << magic_enum::enum_name(permission) << " perm on " << path << std::endl;
}

}  // namespace Errors::OS
