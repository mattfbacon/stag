#pragma once

#include <memory>

#include "ArgParser.hpp"
#include "Commands/Base.hpp"

struct Callbacks : public ArgParser::Callbacks {
	std::unique_ptr<Commands::Base> command_callbacks;
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() const;
};
