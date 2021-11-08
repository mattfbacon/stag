#include <cassert>
#include <string_view>

#include "Errors/ArgParse.hpp"
#include "Errors/Basic.hpp"

#include "common.hpp"

namespace Errors::ArgParse {

void print_option(std::ostream& os, OptType const opt_type, std::string_view const opt_name) {
	switch (opt_type) {
		case OptType::long_opt:
			os << "long option --";
			break;
		case OptType::short_opt:
			os << "short option -";
			break;
	}
	os << opt_name;
}

void ArgumentMismatch::print_to(std::ostream& os) const {
	os << STAG_BINARY_NAME ": " LOGLEVEL_USER ": ";
	switch (what_happened) {
		case Reason::expected_not_provided:
			os << "Expected argument to ";
			break;
		case Reason::provided_not_expected:
			os << "Unexpected argument provided to ";
			break;
	}
	print_option(os, opt_type, opt_name);
	os << std::endl;
}
void UnknownOption::print_to(std::ostream& os) const {
	os << STAG_BINARY_NAME ": " LOGLEVEL_USER ": Unknown ";
	print_option(os, opt_type, opt_name);
	os << std::endl;
}

}  // namespace Errors::ArgParse
