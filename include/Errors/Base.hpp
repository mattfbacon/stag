#pragma once

#include <ostream>
#include <stdexcept>

#include "common.hpp"

#define LOGLEVEL_USER "user"
#define LOGLEVEL_FATAL "fatal"

namespace Errors {

struct Base : public std::exception {
	[[nodiscard]] char const* what() const noexcept override = 0;
	virtual void log() const = 0;
};

}  // namespace Errors
