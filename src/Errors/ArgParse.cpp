#include <cassert>
#include <string_view>

#include "Errors/ArgParse.hpp"
#include "Errors/Basic.hpp"
#include "Logging.hpp"

#include "common.hpp"

namespace Errors::ArgParse {

namespace {

std::string_view option_str(OptType const opt_type) {
	switch (opt_type) {
		case OptType::long_opt:
			return "long option --";
		case OptType::short_opt:
			return "short option -";
		default:
			__builtin_unreachable();
	}
}

}  // namespace

void ArgumentMismatch::log() const {
	switch (what_happened) {
		case Reason::expected_not_provided:
			Logging::user("Expected argument to {}{}", option_str(opt_type), opt_name);
			break;
		case Reason::provided_not_expected:
			Logging::user("Unexpected argument provided to {}{}", option_str(opt_type), opt_name);
			break;
	}
}
void UnknownOption::log() const {
	Logging::user("Unknown {}{}", option_str(opt_type), opt_name);
}

}  // namespace Errors::ArgParse
