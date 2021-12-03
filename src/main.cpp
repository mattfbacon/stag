#include <iostream>
#include <stdlib.h>

#include "ArgParser.hpp"
#include "Callbacks.hpp"
#include "Errors/Base.hpp"
#include "Errors/Commands.hpp"
#include "Filesystem.hpp"
#include "Logging.hpp"

#include "common.hpp"

int main(int const argc, char const* const argv[]) try {
	Callbacks callbacks;
	ArgParser::parse(argc, argv, callbacks);
	if (!callbacks.command_callbacks) {
		throw Errors::Commands::None{};
	}
	Filesystem::ensure_stagspace();
	callbacks.run();
	return EXIT_SUCCESS;
} catch (Errors::Base const& e) {
	e.log();
	return EXIT_FAILURE;
} catch (...) {
	spdlog::dump_backtrace();
	throw;
}
