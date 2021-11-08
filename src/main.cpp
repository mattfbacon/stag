#include <iostream>
#include <stdlib.h>

#include "ArgParser.hpp"
#include "Callbacks.hpp"
#include "Errors/Base.hpp"
#include "Filesystem.hpp"

#include "common.hpp"

int main(int const argc, char const* const argv[]) try {
	Callbacks callbacks;
	ArgParser::parse(argc, argv, callbacks);
	Filesystem::ensure_stagspace();
	callbacks.run();
	return EXIT_SUCCESS;
} catch (Errors::Base const& e) {
	std::clog << e;
	return EXIT_FAILURE;
}
