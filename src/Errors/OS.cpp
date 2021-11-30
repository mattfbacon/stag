#include <magic_enum.hpp>

#include "Errors/Base.hpp"
#include "Errors/OS.hpp"
#include "Logging.hpp"

namespace Errors::OS {

void Permissions::log() const {
	Logging::critical("Missing {} perm on '{}'", magic_enum::enum_name(permission), path);
}

}  // namespace Errors::OS
