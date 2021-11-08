#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "Errors/Base.hpp"

namespace Errors::Basic {

struct User : public ::Errors::Base {
	std::string message;
	explicit User(std::string a_message) : message(std::move(a_message)) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Basic::User";
	}
	void print_to(std::ostream& os) const override;
};

struct Fatal : public ::Errors::Base {
	std::string message;
	explicit Fatal(std::string a_message) : message(std::move(a_message)) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Basic::Fatal";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::Basic
