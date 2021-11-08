#pragma once

#include <string>
#include <vector>

#include "Commands/Base.hpp"

namespace Commands {

struct Open : public Base {
	bool clean = true;
	std::vector<std::string> items_to_open;
	[[nodiscard]] Command what() const override {
		return Command::open;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
};

}  // namespace Commands
