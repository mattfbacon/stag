#include <cassert>
#include <filesystem>
#include <sstream>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/Import.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/OS.hpp"
#include "Filesystem.hpp"
#include "Filesystem/FilesSet.hpp"
#include "Logging.hpp"
#include "Logic/TagDirectory.hpp"
#include "Logic/UUID.hpp"

#include "common.hpp"

namespace Commands {

Result Import::short_option_callback(char const option_name) {
	switch (option_name) {
		case 't':
			return Result::want_argument;
		case 'r':
		case 'n':
			delete_after_importing = option_name == 'r';
			return Result::no_argument;
		default:
			throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Import::short_option_argument_callback(char const option_name, std::string_view value) {
	switch (option_name) {
		case 't':
			all_tags.insert_from_string(value);
			break;
		default:
			assert(false);
			break;
	}
}

Result Import::long_option_callback(std::string_view const option_name) {
	if (option_name == "tags") {
		return short_option_callback('t');
	} else if (option_name == "remove-source") {
		return short_option_callback('r');
	} else if (option_name == "no-remove-source") {
		return short_option_callback('n');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Import::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	assert(option_name == "tags");
	short_option_argument_callback('t', value);
}

void Import::argument_callback(std::string_view value) {
	files_to_import.insert(value);
}

void Import::run() {
	if (files_to_import.empty()) {
		Logging::warning("No files provided");
		return;
	}
#pragma OMP parallel for
	for (auto const& file : files_to_import) {
		Filesystem::check_perms(file, Filesystem::PermsCheck::read);
	}
	std::mutex imported_files_mutex;
	Filesystem::FilesSet imported_files;
#pragma OMP parallel for
	for (auto const& file : files_to_import) {
		std::string imported_filename = Filesystem::import_file(file, delete_after_importing);
		if (!all_tags.empty()) {
			std::lock_guard _lock{ imported_files_mutex };
			imported_files.insert(imported_filename);
		}
	}
#pragma OMP parallel for
	for (auto const& tag : all_tags) {
		Logic::TagDirectory tag_directory{ tag };
		for (auto const& file : imported_files) {
			tag_directory.tag_file(file, false);
		}
	}
}

}  // namespace Commands
