#include <iostream>
#include <magic_enum.hpp>

#include "Errors/Basic.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"

#include "common.hpp"
#include "info.hpp"

namespace Errors::Commands {

void Multiple::log() const {
	if (subcommand == '\0') {
		Logging::user("Multiple commands provided (-{} and then -{})", old_command, new_command);
		print_usage_to(std::cout);
	} else {
		Logging::user("Multiple subcommands provided to -{} (-{} and then -{})", subcommand, old_command, new_command);
	}
}

void None::log() const {
	if (subcommand == '\0') {
		Logging::user("No command provided");
		print_usage_to(std::cout);
	} else {
		Logging::user("No command provided to -{}", subcommand);
	}
}

}  // namespace Errors::Commands
