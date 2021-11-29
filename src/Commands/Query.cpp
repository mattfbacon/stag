#include <cassert>
#include <iostream>
#include <magic_enum.hpp>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/Query.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"
#include "Logic/AllDirectory.hpp"
#include "Logic/TagDirectory.hpp"
#include "Logic/TagsIterator.hpp"
#include "Logic/TagsSet.hpp"

namespace Commands {

Result Query::short_option_callback(char const option_name) {
	if (auto const maybe_subcommand = magic_enum::enum_cast<Subcommand>(option_name); maybe_subcommand.has_value() && *maybe_subcommand != Subcommand::none) {
		if (subcommand != Subcommand::none) {
			throw Errors::Commands::Multiple{ static_cast<char>(subcommand), option_name, static_cast<char>(this->what()) };
		}
		subcommand = *maybe_subcommand;
		return Result::no_argument;
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Query::short_option_argument_callback(char const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

Result Query::long_option_callback(std::string_view const option_name) {
	if (option_name == "list-tags") {
		return short_option_callback('t');
	} else if (option_name == "stats") {
		return short_option_callback('s');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Query::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

void Query::argument_callback(std::string_view value) {
	files.emplace(value);
}

namespace {

void list_all_tags() {
	Logic::TagsIterator tags_iterator;
	if (tags_iterator.empty()) {
		std::clog << "(No tags in the " STAG_PROGRAM_NAME "-space)" << std::endl;
	} else {
		for (auto const& tag : tags_iterator) {
			std::cout << tag.name() << '\n';
		}
	}
}

void list_file_tags(std::filesystem::path const& file) {
	Logic::TagsSet file_tags;
	bool has_outputted = false;
	for (auto const& tag : Logic::TagsIterator{}) {
		if (tag.has_file(file)) {
			has_outputted = true;
			std::cout << tag.name() << '\n';
		}
	}
	if (!has_outputted) {
		std::clog << "(File has no tags)" << std::endl;
	}
}

std::unordered_map<std::string, size_t> count_tags_for_files(Filesystem::FilesSet const& files) {
	std::unordered_map<std::string, size_t> ret;
	for (auto const& tag : Logic::TagsIterator{}) {
		for (auto const& file : files) {
			if (tag.has_file(file)) {
				++ret[tag.name()];
			}
		}
	}
	return ret;
}

void list_tags_summary(Filesystem::FilesSet const& files) {
	// for each tag, the number of files that has that tag
	auto const tag_counts = count_tags_for_files(files);
	if (tag_counts.empty()) {
		std::clog << "(Files have no tags)" << std::endl;
	} else {
		for (auto const& [tag_name, count] : tag_counts) {  // cppcheck-suppress unassignedVariable
			std::cout << count << '\t' << tag_name << '\n';
		}
	}
}

}  // namespace

void Query::run_list_tags() {
	if (files.empty()) {  // list all tags
		list_all_tags();
	} else if (files.size() == 1) {
		auto const expanded_file = Logic::AllDirectory::expand_file_abbreviation(*files.begin());
		list_file_tags(expanded_file);
	} else {
		Filesystem::FilesSet expanded_files;
		for (auto const& file : files) {
			expanded_files.insert_expanded(file);
		}
		list_tags_summary(expanded_files);
	}
}

void Query::run_stats() {
	if (!files.empty()) {
		Logging::warn("Files given to -Qs are ignored");
	}

	auto const number_of_files = Logic::AllDirectory::num_files();
	size_t number_of_tags = 0;
	size_t total_number_of_file_to_tag_associations = 0;

	for (auto const& tag : Logic::TagsIterator{}) {
		++number_of_tags;
		total_number_of_file_to_tag_associations += tag.num_files();
	}

	std::cout << "Number of files: " << number_of_files << '\n';
	std::cout << "Number of tags: " << number_of_tags << '\n';
	std::cout << "Average number of tags per file: " << (static_cast<double>(total_number_of_file_to_tag_associations) / static_cast<double>(number_of_files)) << '\n';
	std::cout << "Total number of file-to-tag associations: " << total_number_of_file_to_tag_associations << '\n';
}

void Query::run() {
	if (subcommand == Subcommand::none) {
		subcommand = Subcommand::list_tags;
	}
	switch (subcommand) {
		case Subcommand::list_tags:
			return run_list_tags();
		case Subcommand::stats:
			return run_stats();
		case Subcommand::none:
			__builtin_unreachable();
	}
}

}  // namespace Commands
