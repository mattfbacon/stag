#include <magic_enum.hpp>

#include "Errors/Basic.hpp"
#include "Errors/Commands.hpp"

#include "common.hpp"

namespace Errors::Commands {

void Multiple::print_to(std::ostream& os) const {
	// if subcommand: "Multiple subcommands provided to -Q (-t and then -s)"
	// if no subcommand: "Multiple commands provided (-Q and then -T)"
	os << STAG_BINARY_NAME ": " LOGLEVEL_USER ": Multiple ";
	if (subcommand != '\0') {
		os << "sub";
	}
	os << "commands provided";
	if (subcommand != '\0') {
		os << " to -" << subcommand;
	}
	os << " (";
	os << '-' << old_command << " and then ";
	os << '-' << new_command << ')' << std::endl;
}

void None::print_to(std::ostream& os) const {
	// if subcommand: "No command provided"
	// if no subcommand: "No subcommand provided to -Q"
	os << STAG_BINARY_NAME ": " LOGLEVEL_USER ": No ";
	if (subcommand != '\0') {
		os << "sub";
	}
	os << "command provided";
	if (subcommand != '\0') {
		os << " to -" << subcommand;
	}
	os << std::endl;
}

}  // namespace Errors::Commands
