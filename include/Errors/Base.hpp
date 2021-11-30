#pragma once

#include <ostream>
#include <stdexcept>

#include "common.hpp"

namespace Errors {

struct Base : public std::exception {
	[[nodiscard]] char const* what() const noexcept override = 0;
	virtual void log() const = 0;
};

}  // namespace Errors
