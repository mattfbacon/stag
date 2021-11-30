#include <cassert>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/Views.hpp"
#include "Errors/ArgParse.hpp"
#include "Logging.hpp"
#include "View/View.hpp"

namespace Commands {

Result Views::short_option_callback(char const option_name) {
	switch (option_name) {
		case 'f':
			always_generate = true;
			return Result::no_argument;
		default:
			throw Errors::ArgParse::UnknownOption{ option_name };
	}
}
void Views::short_option_argument_callback(char const option_name, std::string_view value) {
	Logging::debug("{}::{}_option_argument_callback should have been unreachable (called with '{}' and '{}')", "Views", "short", option_name, value);
	(void)option_name;
	(void)value;
	assert(false);
}
Result Views::long_option_callback(std::string_view const option_name) {
	if (option_name == "force") {
		return short_option_callback('f');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}
void Views::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	Logging::debug("{}::{}_option_argument_callback should have been unreachable (called with '{}' and '{}')", "Views", "long", option_name, value);
	(void)option_name;
	(void)value;
	assert(false);
}
void Views::argument_callback(std::string_view value) {
	viewspecs.emplace_back(value);
}
void Views::run() {
	for (auto const& viewspec : viewspecs) {
		View::View view{ viewspec, false, always_generate };
	}
}

}  // namespace Commands
