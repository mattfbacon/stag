#include <iostream>

#include "Errors/Basic.hpp"
#include "Logging.hpp"

#include "common.hpp"
#include "info.hpp"

namespace Errors::Basic {

void User::log() const {
	Logging::user("{}", message);
}
void Fatal::log() const {
	Logging::critical("{}", message);
	print_usage_to(std::clog);
}

}  // namespace Errors::Basic
