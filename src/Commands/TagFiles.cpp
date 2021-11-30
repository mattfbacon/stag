#include <cassert>
#include <magic_enum.hpp>
#include <mutex>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/TagFiles.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"
#include "Logic/AllDirectory.hpp"
#include "Logic/TagDirectory.hpp"

namespace Commands {

Result TagFiles::short_option_callback(char const option_name) {
	switch (option_name) {
		case 'r':
		case 'n':
			renumber = option_name == 'r';
			return Result::no_argument;
		default:
			if (auto const maybe_subcommand = magic_enum::enum_cast<Subcommand>(option_name); maybe_subcommand.has_value() && *maybe_subcommand != Subcommand::none) {
				if (subcommand == Subcommand::none) {
					throw Errors::Commands::Multiple{ static_cast<char>(subcommand), option_name, static_cast<char>(this->what()) };
				}
				subcommand = *maybe_subcommand;
				return Result::no_argument;
			} else {
				throw Errors::ArgParse::UnknownOption{ option_name };
			}
	}
}

void TagFiles::short_option_argument_callback(char const option_name, std::string_view value) {
	Logging::debug("{}::{}_option_argument_callback should have been unreachable (called with '{}' and '{}')", "TagFiles", "short", option_name, value);
	(void)option_name;
	(void)value;
	assert(false);
}

Result TagFiles::long_option_callback(std::string_view const option_name) {
	if (option_name == "tag") {
		return short_option_callback('t');
	} else if (option_name == "untag") {
		return short_option_callback('u');
	} else if (option_name == "renumber") {
		return short_option_callback('r');
	} else if (option_name == "no-renumber") {
		return short_option_callback('n');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void TagFiles::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	Logging::debug("{}::{}_option_argument_callback should have been unreachable (called with '{}' and '{}')", "TagFiles", "long", option_name, value);
	(void)option_name;
	(void)value;
	assert(false);
}

void TagFiles::argument_callback(std::string_view value) {
	if (value.rfind('.') == std::string_view::npos) {
		tags.emplace(value);
	} else {
		files.emplace(value);
	}
}

void TagFiles::run() {
	Filesystem::FilesSet expanded_files;
	{
		std::mutex expanded_files_mutex;
		for (auto const& abbreviation : files) {
			auto expanded = Logic::AllDirectory::expand_file_abbreviation(abbreviation);
			{
				std::lock_guard _lock{ expanded_files_mutex };
				// FIXME: maybe write another insert method for already-expanded path?
				expanded_files.insert(std::move(expanded));  // NOLINT(performance-move-const-arg) - may write such an overload in the future
			}
		}
	}
#pragma GCC poison files
	if (subcommand == Subcommand::none) {
		subcommand = Subcommand::tag_files;
	}
	switch (subcommand) {
		case Subcommand::tag_files:
#pragma OMP parallel for
			for (auto const& tag : tags) {
				Logic::TagDirectory tag_dir{ tag };
				for (auto const& file : expanded_files) {
					tag_dir.tag_file(file);
				}
			}
			break;
		case Subcommand::untag_files:
#pragma OMP parallel for
			for (auto const& tag : tags) {
				Logic::TagDirectory tag_dir{ tag };
				tag_dir.untag_files(expanded_files, renumber);
			}
			break;
		case Subcommand::none:
			__builtin_unreachable();
	}
}

}  // namespace Commands
