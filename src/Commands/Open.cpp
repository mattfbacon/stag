#include <cassert>
#include <string_view>
#include <sys/wait.h>

#include "Callbacks.hpp"
#include "Commands/Open.hpp"
#include "Errors/Abbreviation.hpp"
#include "Errors/ArgParse.hpp"
#include "Logging.hpp"
#include "Logic/AllDirectory.hpp"
#include "Logic/TagDirectory.hpp"
#include "View/View.hpp"

namespace Commands {

Result Open::short_option_callback(char const option_name) {
	switch (option_name) {
		case 'c':
			clean = true;
			return Result::no_argument;
		case 'n':
			clean = false;
			return Result::no_argument;
		default:
			throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Open::short_option_argument_callback(char const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

Result Open::long_option_callback(std::string_view const option_name) {
	if (option_name == "clean") {
		return short_option_callback('c');
	} else if (option_name == "no-clean") {
		return short_option_callback('n');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Open::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

void Open::argument_callback(std::string_view const value) {
	items_to_open.emplace_back(value);
}

enum class ItemType {
	tag,
	file,
	view,
};

/**
 * If there is a character that's not a tag character,
 * then if that character not a period, it's a view;
 * otherwise, it's a file.
 */
constexpr ItemType item_type(std::string_view const item) {
	bool has_had_period = false;
	for (auto const character : item) {
		if (util::char_traits::is_tagchar(character)) {
			continue;
		}
		has_had_period = true;
		if (character != '.') {
			return ItemType::view;
		}
	}
	return has_had_period ? ItemType::file : ItemType::tag;
}
static_assert(item_type("abc def") == ItemType::tag);
static_assert(item_type("abc.def") == ItemType::file);
static_assert(item_type("abc+def") == ItemType::view);

void open_file(std::filesystem::path const& path) {
	auto const pid = ::fork();
	if (pid == 0) {  // child
		auto path_str = path.string();
		std::string prog_name = "xdg-open";
		std::array<char*, 3> args{ prog_name.data(), path_str.data(), nullptr };
		if (::execvp(prog_name.c_str(), args.data()) < 0) {
			static constexpr int EXIT_COMMAND_INVOKED_CANNOT_EXECUTE = 126;
			exit(EXIT_COMMAND_INVOKED_CANNOT_EXECUTE);
		}
	} else {  // parent
		int status;
		if (::waitpid(pid, &status, 0) < 0) {
			throw std::system_error{ errno, std::system_category() };
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) > 0) {
			Logging::warning("Running subcommand failed with exit code ", WEXITSTATUS(status));
		}
	}
}

void open_tag(std::string_view const tag_name) {
	Logic::TagDirectory tag_dir{ tag_name };
	if (!tag_dir.empty()) {
		open_file(*tag_dir.begin());
	}
}

void open_abbr_file(std::string_view const abbr) {
	try {
		auto const expanded = Logic::AllDirectory::expand_file_abbreviation(abbr);
		open_file(expanded);
	} catch (Errors::Abbreviation::Base const& e) {
		e.print_to(std::clog);
	}
}

void open_view(std::string_view const viewspec, bool const clean) {
	View::View view{ viewspec, clean };
	if (auto const view_begin = view.begin(); view_begin != view.end()) {
		open_file(*view_begin);
	}
}

void Open::run() {
	for (auto const& item_to_open : items_to_open) {
		switch (item_type(item_to_open)) {
			case ItemType::tag:
				open_tag(item_to_open);
				break;
			case ItemType::file:
				open_abbr_file(item_to_open);
				break;
			case ItemType::view:
				open_view(item_to_open, clean);
				break;
		}
	}
}

}  // namespace Commands
