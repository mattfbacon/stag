#pragma once

#include "Commands/Base.hpp"
#include "View/Viewspec.hpp"

namespace Commands {

struct Views : public Base {
	bool always_generate = false;
	std::vector<std::string> viewspecs;
	[[nodiscard]] Command what() const override {
		return Command::views;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
};

}  // namespace Commands
