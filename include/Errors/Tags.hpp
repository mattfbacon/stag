#pragma once

#include <ostream>
#include <string>
#include <utility>

#include "Errors/Base.hpp"

namespace Errors::Tags {

struct Base : public ::Errors::Base {};

struct Exists : public Base {
	std::string tag_name;
	explicit Exists(std::string a_tag_name) : tag_name(std::move(a_tag_name)) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Tags::Exists";
	}
	void print_to(std::ostream& os) const override;
};

struct DoesNotExist : public Base {
	std::string tag_name;
	explicit DoesNotExist(std::string a_tag_name) : tag_name(std::move(a_tag_name)) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Tags::DoesNotExist";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::Tags
