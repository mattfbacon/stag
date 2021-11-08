#pragma once

#include "Commands/Base.hpp"
#include "Logic/TagsSet.hpp"

namespace Commands {

struct Fix : public Base {
	Logic::TagsSet tags;
	bool inverse = false;
	enum class Subcommand : char {
		none = '\0',
		renumber_all_tags = 'r',
		fix_broken_links = 'b',
	} subcommand = Subcommand::none;
	bool force = false;
	[[nodiscard]] Command what() const override {
		return Command::fix;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
};

}  // namespace Commands
