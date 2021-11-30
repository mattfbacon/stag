#include <cassert>
#include <charconv>
#include <sstream>

#include "Errors/Tags.hpp"
#include "Filesystem.hpp"
#include "Logging.hpp"
#include "Logic/TagDirectory.hpp"
#include "Logic/TagIterator.hpp"

namespace Logic {

void TagDirectory::find_next_number() {
	// optimization
	std::filesystem::path num_path = this_tag_path / std::to_string(current_number);
	do {
		++current_number;
		num_path.replace_filename(std::to_string(current_number));
	} while (std::filesystem::exists(num_path));
}

std::filesystem::path TagDirectory::index_to_path(tag_index_t const idx) const {
	return this_tag_path / std::to_string(idx);
}
std::filesystem::path TagDirectory::current_path() const {
	return index_to_path(current_number);
}

bool TagDirectory::valid() const {
	return !this_tag_path.empty();
}

TagDirectory::TagDirectory(std::filesystem::path a_this_tag_path, std::string a_tag_name)
	: this_tag_path(std::move(a_this_tag_path)), tag_name(std::move(a_tag_name)) {
	std::filesystem::create_directory(this_tag_path);
	find_next_number();
}

TagDirectory::TagDirectory(std::string&& a_tag_name) : TagDirectory{ Filesystem::path_tags / a_tag_name, std::move(a_tag_name) } {}

TagDirectory TagDirectory::from_path(std::filesystem::path actual_directory) {
	auto given_tag_name = actual_directory.filename().string();
	return { std::move(actual_directory), std::move(given_tag_name) };
}

bool TagDirectory::exists(std::string_view const tag_name) {
	return std::filesystem::exists(Filesystem::path_tags / tag_name);
}

std::filesystem::path TagDirectory::dereference(std::filesystem::path const& file) const {
	auto ret = this_tag_path / std::filesystem::read_symlink(file);
	Logging::trace("Dereferencing tag item '{}' with result '{}'", file, ret);
	return ret;
}

std::string const& TagDirectory::name() const {
	return tag_name;
}

TagDirectory::RemoveResult TagDirectory::remove(std::string_view const tag_name) {
	Logging::trace("Removing tag '{}'", tag_name);
	auto const result = std::filesystem::remove_all(Filesystem::path_tags / tag_name);
	// remove_all returns the number of entries that were removed
	if (result > 0) {
		return RemoveResult::removed;
	} else {
		return RemoveResult::did_not_exist;
	}
}

void TagDirectory::rename(std::string_view new_name) {
	Logging::trace("Renaming tag from '{}' to '{}'", this_tag_path.filename(), new_name);
	auto new_path = Filesystem::path_tags / new_name;
	if (std::filesystem::exists(new_path)) {
		throw Errors::Tags::Exists{ std::string{ new_name } };
	}
	std::filesystem::rename(this_tag_path, new_path);
	this_tag_path = std::move(new_path);
	tag_name = new_name;
}

size_t TagDirectory::num_files() const {
	size_t ret = 0;
	for (auto const& unused : TagIterator{ this_tag_path }) {
		(void)unused;
		++ret;
	}
	return ret;
}

std::optional<tag_index_t> TagDirectory::index_from_str(std::string_view const str) {
	tag_index_t ret;
	auto const result = std::from_chars(str.begin(), str.end(), ret);
	// full string wasn't parsed (e.g., 123garbage -> 123) OR there was an error
	if (result.ptr != str.end() || result.ec != std::errc{}) {
		return std::nullopt;
	}
	return { ret };
}

std::optional<tag_index_t> TagDirectory::find_file(std::filesystem::path const& file_name) const {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Finding file in tag '{}' with filename '{}'", this_tag_name, file_name);
	for (auto const& tagged_file : TagIterator{ this_tag_path }) {
		auto dereferenced = dereference(tagged_file);
		Logging::trace("Find('{}' in '{}'): Checking '{}' which points to '{}'", file_name, this_tag_name, tagged_file, dereferenced);
		if (std::filesystem::equivalent(dereferenced, file_name)) {
			return index_from_str(tagged_file.filename().string());
		}
	}
	return std::nullopt;
}

bool TagDirectory::has_file(std::filesystem::path const& file_name) const {
	return find_file(file_name).has_value();
}

tag_index_t TagDirectory::tag_file(std::string name_in_all, bool const ensure_uniqueness) {
	return tag_file(Filesystem::path_all / std::move(name_in_all), ensure_uniqueness);
}
tag_index_t TagDirectory::tag_file(std::filesystem::path const& file_path, bool const ensure_uniqueness) {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Giving tag '{}' to file '{}' (ensure uniqueness? {})", this_tag_name, file_path, ensure_uniqueness);
	if (ensure_uniqueness) {
		if (auto const maybe_pos = find_file(file_path); maybe_pos.has_value()) {
			Logging::debug("File to be tagged ({}) already has tag '{}' (index: {})", file_path, this_tag_name, *maybe_pos);
			return *maybe_pos;
		}
	}
	auto const number = current_number;
	std::filesystem::create_symlink(file_path.lexically_relative(std::filesystem::current_path()).lexically_relative(this_tag_path), current_path());
	find_next_number();
	return number;
}

TagDirectory::ReplacementResult TagDirectory::replace_tagged_file(std::filesystem::path const& file_path, std::filesystem::path const& new_path, bool const ensure_uniqueness) {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Attempting to replace tagged file '{}' with '{}' (tag: {})", file_path, new_path, this_tag_name);
	if (ensure_uniqueness) {
		if (auto const maybe_pos = find_file(new_path); maybe_pos.has_value()) {
			Logging::debug("New file ({}), to replace '{}', already has tag '{}' (index: {})", new_path, file_path, this_tag_name, *maybe_pos);
			return ReplacementResult::not_replaced;
		}
	}
	std::filesystem::create_symlink(new_path.lexically_relative(std::filesystem::current_path()).lexically_relative(this_tag_path), file_path);
	return ReplacementResult::replaced;
}

void TagDirectory::untag_file(std::filesystem::path const& name_in_all, bool const renumber) {
	return untag_files(Filesystem::FilesSet{ name_in_all }, renumber);
}

void TagDirectory::untag_files(Filesystem::FilesSet const& names_in_all, bool const renumber) {
	Logging::trace("Removing tag '{}' from file{} '{}'", this_tag_path.filename(), names_in_all.size() == 1 ? "" : "s", fmt::join(names_in_all, "', '"));
	auto num_tagged_files = this->num_files();
	struct {
		tag_index_t current = 1, new_ = 1;
	} counters;
	// optimization
	auto current_name_storage = index_to_path(counters.current);
	auto new_name_storage = index_to_path(counters.new_);
	while (counters.new_ <= num_tagged_files) {
		if (std::filesystem::exists(current_name_storage)) {
			auto const link_source = dereference(current_name_storage);
			if (auto const iter = names_in_all.find(link_source); iter != names_in_all.end()) {
				std::filesystem::remove(current_name_storage);
				if (names_in_all.empty() && !renumber) {
					break;
				}
				--num_tagged_files;
			} else {
				if (renumber) {
					std::filesystem::rename(current_name_storage, new_name_storage);
				}
				++counters.new_;
				new_name_storage.replace_filename(std::to_string(counters.new_));
			}
		}
		++counters.current;
		current_name_storage.replace_filename(std::to_string(counters.current));
	}
}

void TagDirectory::untag_file(tag_index_t const index) {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Removing tag '{}' from index {}", this_tag_name, index);
	std::filesystem::remove(index_to_path(index));
	// if the slot we just freed is before the current first free slot, update the first free slot.
	if (index < current_number) {
		Logging::debug("Updating current index for '{}' as index removed ({}) was before current ({})", this_tag_name, index, current_number);
		current_number = index;
	}
}

void TagDirectory::renumber() {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Renumbering tag '{}'", this_tag_name);
	auto num_tagged_files = this->num_files();
	struct {
		tag_index_t current = 1, new_ = 1;
	} counters;
	// optimization
	auto current_name_storage = index_to_path(counters.current);
	auto new_name_storage = index_to_path(counters.new_);
	while (counters.new_ <= num_tagged_files) {
		current_name_storage.replace_filename(std::to_string(counters.current));
		if (std::filesystem::exists(current_name_storage)) {
			new_name_storage.replace_filename(std::to_string(counters.new_));
			Logging::trace("Renumber({}): move at {} -> {}", this_tag_name, counters.current, counters.new_);
			std::filesystem::rename(current_name_storage, new_name_storage);
			++counters.new_;
		} else {
			Logging::trace("Renumber({}): hole at {}", this_tag_name, counters.current);
		}
		++counters.current;
	}
}
void TagDirectory::strip_broken() {
	auto const this_tag_name = this_tag_path.filename();
	Logging::trace("Stripping broken links from tag {}", this_tag_name);
	auto num_tagged_files = this->num_files();
	struct {
		tag_index_t current = 1, new_ = 1;
	} counters;
	// optimization
	auto current_name_storage = index_to_path(counters.current);
	auto new_name_storage = index_to_path(counters.new_);
	while (counters.new_ <= num_tagged_files) {
		if (std::filesystem::exists(current_name_storage)) {
			auto const link_source = dereference(current_name_storage);
			if (!std::filesystem::exists(link_source)) {
				Logging::trace("StripBroken({}): broken at {}", this_tag_name, counters.current);
				std::filesystem::remove(current_name_storage);
				--num_tagged_files;
			} else {
				Logging::trace("StripBroken({}): valid at {}", this_tag_name, counters.current);
				std::filesystem::rename(current_name_storage, new_name_storage);
				++counters.new_;
				new_name_storage.replace_filename(std::to_string(counters.new_));
			}
		} else {
			Logging::trace("StripBroken({}): hole at {}", this_tag_name, counters.current);
		}
		++counters.current;
		current_name_storage.replace_filename(std::to_string(counters.current));
	}
}

TagIterator TagDirectory::begin() const {
	return TagIterator{ this_tag_path };
}
TagIterator TagDirectory::end() const {  // NOLINT(readability-convert-member-functions-to-static)
	return TagIterator{};
}
bool TagDirectory::empty() const {
	return begin() != end();
}

}  // namespace Logic
