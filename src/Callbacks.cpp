#include <cassert>
#include <cstdlib>
#include <iostream>
#include <magic_enum.hpp>

#include "Callbacks.hpp"
#include "Commands/Base.hpp"
#include "Commands/Fix.hpp"
#include "Commands/Import.hpp"
#include "Commands/MetaTag.hpp"
#include "Commands/Open.hpp"
#include "Commands/Query.hpp"
#include "Commands/TagFiles.hpp"
#include "Commands/Views.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"

#include "common.hpp"
#include "info.hpp"

namespace ArgErr = Errors::ArgParse;
namespace CmdErr = Errors::Commands;

#define INDENT "  "

namespace Commands {

std::unique_ptr<Base> make(Command const command) {
	switch (command) {
		case Command::import:
			return std::make_unique<Import>();
		case Command::query:
			return std::make_unique<Query>();
		case Command::tag_files:
			return std::make_unique<TagFiles>();
		case Command::metatag:
			return std::make_unique<MetaTag>();
		case Command::fix:
			return std::make_unique<Fix>();
		case Command::views:
			return std::make_unique<Views>();
		case Command::open:
			return std::make_unique<Open>();
		case Command::none:
			Logging::debug("Trying to make a Command of type none");
			assert(false);
			return nullptr;
	}
	__builtin_unreachable();
}

}  // namespace Commands

using Result = Callbacks::Result;

Result Callbacks::short_option_callback(char const option_name) {
	if (util::char_traits::is_upper(option_name)) {  // command
		auto const maybe_command = magic_enum::enum_cast<Command>(option_name);
		if (maybe_command) {
			if (command_callbacks) {
				throw CmdErr::Multiple{ static_cast<char>(command_callbacks->what()), option_name };
			}
			command_callbacks = Commands::make(*maybe_command);
			return Result::no_argument;
		} else {
			throw ArgErr::UnknownOption{ option_name };
		}
	} else {
		if (!command_callbacks) {
			throw CmdErr::None{};
		}
		Logging::trace("Passing short option '{}' to subcommand", option_name);
		return command_callbacks->short_option_callback(option_name);
	}
}

void Callbacks::short_option_argument_callback(char const option_name, std::string_view const value) {
	if (!command_callbacks) {
		throw CmdErr::None{};
	}
	Logging::trace("Passing argument '{}' for short option '{}' to subcommand", value, option_name);
	return command_callbacks->short_option_argument_callback(option_name, value);
}

std::unordered_map<std::string_view, std::function<Result(void)>> const global_long_options{
	// clang-format off
	{ "help", [] { print_usage_to(std::cout); std::exit(EXIT_SUCCESS); return Result::no_argument; } },
	{ "version", [] { print_version(); std::exit(EXIT_SUCCESS); return Result::no_argument; } },
	{ "verbose", [] { Logging::set_level_from_string("trace"); return Result::no_argument; } },
	{ "log-level", [] { return Result::want_argument; } },
	// clang-format on
};
Result Callbacks::long_option_callback(std::string_view const option_name) {
	if (auto const maybe_callback = util::maybe_get(global_long_options, option_name); maybe_callback.has_value()) {
		return (*maybe_callback)();
	} else {
		if (!command_callbacks) {
			throw Errors::Commands::None{};
		}
		Logging::trace("Passing long option '{}' to subcommand", option_name);
		return command_callbacks->long_option_callback(option_name);
	}
}

void Callbacks::long_option_argument_callback(std::string_view const option_name, std::string_view const value) {
	if (option_name == "log-level") {
		Logging::set_level_from_string(value);
		return;
	}
	if (!command_callbacks) {
		throw Errors::Commands::None{};
	}
	Logging::trace("Passing argument '{}' for long option '{}' to subcommand", value, option_name);
	return command_callbacks->long_option_argument_callback(option_name, value);
}

void Callbacks::argument_callback(std::string_view const value) {
	if (!command_callbacks) {
		throw Errors::Commands::None{};
	}
	Logging::trace("Passing argument '{}' to subcommand", value);
	return command_callbacks->argument_callback(value);
}

void Callbacks::run() const {
	if (!command_callbacks) {
		throw Errors::Commands::None{};
	}
	command_callbacks->run();
}
