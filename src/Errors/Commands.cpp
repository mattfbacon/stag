#include <magic_enum.hpp>

#include "Errors/Basic.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"

#include "common.hpp"

namespace Errors::Commands {

void Multiple::log() const {
	if (subcommand == '\0') {
		Logging::user("Multiple commands provided (-{} and then -{})", old_command, new_command);
	} else {
		Logging::user("Multiple subcommands provided to -{} (-{} and then -{})", subcommand, old_command, new_command);
	}
}

void None::log() const {
	if (subcommand == '\0') {
		Logging::user("No command provided");
	} else {
		Logging::user("No command provided to -{}", subcommand);
	}
}

}  // namespace Errors::Commands
