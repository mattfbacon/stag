#pragma once

#include "Commands/Base.hpp"
#include "Filesystem/FilesSet.hpp"

namespace Commands {

struct Query : public Base {
	enum class Subcommand : char {
		none = '\0',
		list_tags = 't',
		stats = 's',
	} subcommand = Subcommand::none;
	Filesystem::FilesAbbrSet files;
	Command what() const override {
		return Command::query;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
protected:
	void run_list_tags();
	void run_stats();
};

}  // namespace Commands
