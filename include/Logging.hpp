#pragma once

#include <iostream>

#include "common.hpp"

namespace Logging {

template <typename... Items>
void warning(Items const&... items) {
	std::clog << STAG_BINARY_NAME ": warning: ";
	(std::clog << ... << items) << std::endl;
}

template <typename... Items>
void info(Items const&... items) {
	std::clog << STAG_BINARY_NAME ": info: ";
	(std::clog << ... << items) << std::endl;
}

}  // namespace Logging
