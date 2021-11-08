#pragma once

#include <memory>

#include "ArgParser.hpp"

enum class Command : char {
	none = '\0',
	import = 'U',
	query = 'Q',
	tag_files = 'T',
	metatag = 'R',
	fix = 'F',
	views = 'V',
	open = 'O',
};

namespace Commands {
using Result = ArgParser::Callbacks::Result;

struct Base : public ArgParser::Callbacks {
	[[nodiscard]] virtual Command what() const = 0;
	virtual ~Base() = default;
	virtual void run() = 0;
};

}  // namespace Commands
