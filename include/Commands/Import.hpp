#pragma once

#include "Commands/Base.hpp"
#include "Filesystem/FilesSet.hpp"
#include "Logic/TagsSet.hpp"

namespace Commands {

struct Import : public Base {
	Logic::TagsSet all_tags;
	Filesystem::FilesSet files_to_import;
	bool delete_after_importing = false;
	Command what() const override {
		return Command::import;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
};

}  // namespace Commands
