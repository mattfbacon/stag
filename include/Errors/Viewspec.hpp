#pragma once

#include <tao/pegtl/position.hpp>
#include <vector>

#include "Errors/Base.hpp"

namespace Errors::Viewspec {

struct ParseFailure : public ::Errors::Base {
	std::string message;
	std::vector<tao::pegtl::position> positions;
	ParseFailure(std::string a_message, std::vector<tao::pegtl::position> a_positions) : message(std::move(a_message)), positions(std::move(a_positions)) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Viewspec::ParseFailure";
	}
	void log() const override;
};

}  // namespace Errors::Viewspec
