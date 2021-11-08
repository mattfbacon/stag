#pragma once

#include <ostream>
#include <string>

#include "Callbacks.hpp"
#include "Errors/Base.hpp"

namespace Errors::Commands {

struct Multiple : public ::Errors::Base {
	char old_command, new_command, subcommand;
	Multiple(char const a_old_command, char const a_new_command, char const a_subcommand = '\0')
		: old_command(a_old_command), new_command(a_new_command), subcommand(a_subcommand) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Commands::Multiple";
	}
	void print_to(std::ostream& os) const override;
};

struct None : public ::Errors::Base {
	char subcommand;
	explicit None(char const a_subcommand = '\0') : subcommand(a_subcommand) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::Commands::None";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::Commands
