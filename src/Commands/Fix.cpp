#include <cassert>
#include <filesystem>
#include <magic_enum.hpp>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/Fix.hpp"
#include "Errors/Abbreviation.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/Commands.hpp"
#include "Logging.hpp"
#include "Logic/AllDirectory.hpp"
#include "Logic/TagsIterator.hpp"

namespace Commands {

Result Fix::short_option_callback(char const option_name) {
	switch (option_name) {
		case 'v':
			inverse = true;
			return Result::no_argument;
		case 'f':
			force = true;
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

void Fix::short_option_argument_callback(char const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

Result Fix::long_option_callback(std::string_view const option_name) {
	if (option_name == "renumber") {
		return short_option_callback('r');
	} else if (option_name == "broken") {
		return short_option_callback('b');
	} else if (option_name == "force") {
		return short_option_callback('f');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void Fix::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

void Fix::argument_callback(std::string_view const value) {
	tags.emplace(value);
}

namespace {

template <bool Parallel>
void for_each_tag(Logic::TagsSet const& tags, bool const inverse, std::function<void(Logic::TagDirectory& tag_dir)> const& func) {
	if (inverse) {
#pragma OMP parallel for if (Parallel)
		for (auto& tag : Logic::TagsIterator{}) {
			if (tags.contains(tag.name())) {
				continue;
			}
			func(tag);
		}
	} else {
#pragma OMP parallel for if (Parallel)
		for (auto const& tag_name : tags) {
			if (!Logic::TagDirectory::exists(tag_name)) {
				Logging::warn("Tag '{}' does not exist", tag_name);
				continue;
			}
			auto tag = Logic::TagDirectory{ tag_name };
			func(tag);
		}
	}
}

void renumber_tags(Logic::TagsSet const& tags, bool const inverse) {
	for_each_tag<true>(tags, inverse, [](Logic::TagDirectory& tag_dir) {
		tag_dir.renumber();
	});
}

/**
 * I want this to be kind of like `git add -p`.
 * For each tag, for each broken link, we offer some choices for what to do.
 */
void fix_broken_links(Logic::TagsSet const& tags, bool const inverse, bool const force) {
	if (!force) {
		Logging::warn("-Fb without -f is not yet implemented");
		return;
	}
	for_each_tag<true>(tags, inverse, [](Logic::TagDirectory& tag_dir) {
		tag_dir.strip_broken();
	});
}

}  // namespace

void Fix::run() {
	if (tags.empty() && !inverse) {
		inverse = true;  // do all tags
	}
	switch (subcommand) {
		case Subcommand::none:
			throw Errors::Commands::None{ static_cast<char>(this->what()) };
		case Subcommand::renumber_all_tags:
			return renumber_tags(tags, inverse);
		case Subcommand::fix_broken_links:
			return fix_broken_links(tags, inverse, force);
	}
}

}  // namespace Commands
