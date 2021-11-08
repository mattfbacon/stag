#pragma once

#include "Commands/Base.hpp"
#include "Filesystem/FilesSet.hpp"
#include "Logic/TagsSet.hpp"

namespace Commands {

struct TagFiles : public Base {
	enum class Subcommand {
		none = '\0',
		tag_files = 't',
		untag_files = 'u',
		// clang-format off
	} subcommand = Subcommand::none;
	// clang-format on
	bool renumber = false;
	Logic::TagsSet tags;
	Filesystem::FilesAbbrSet files;
public:
	Command what() const override {
		return Command::tag_files;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
};

}  // namespace Commands
