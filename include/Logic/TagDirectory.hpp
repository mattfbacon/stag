#pragma once

#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "Filesystem.hpp"
#include "Filesystem/FilesSet.hpp"
#include "Logic/TagIterator.hpp"

namespace Logic {

using tag_index_t = size_t;

class TagDirectory {
protected:
	tag_index_t current_number = 0;
	std::filesystem::path this_tag_path;
	std::string tag_name;

	void find_next_number();
	std::filesystem::path index_to_path(tag_index_t const idx) {
		return this_tag_path / std::to_string(idx);
	}
	std::filesystem::path current_path() {
		return index_to_path(current_number);
	}
	TagDirectory(std::filesystem::path a_this_tag_path, std::string a_tag_name);
public:
	enum class RemoveResult {
		removed,
		did_not_exist,
	};

	TagDirectory() = default;
	[[nodiscard]] bool valid() const {
		return !this_tag_path.empty();
	}
	explicit TagDirectory(std::string_view a_tag_name) : TagDirectory{ std::string{ a_tag_name } } {}
	explicit TagDirectory(std::string&& a_tag_name);
	static TagDirectory from_path(std::filesystem::path actual_directory) {
		auto given_tag_name = actual_directory.filename().string();
		return { std::move(actual_directory), std::move(given_tag_name) };
	}
	[[nodiscard]] static bool exists(std::string_view tag_name);

	[[nodiscard]] auto dereference(std::filesystem::path const& file) const {
		return this_tag_path / std::filesystem::read_symlink(file);
	}

	[[nodiscard]] std::string const& name() const {
		return tag_name;
	}

	[[nodiscard]] static std::optional<tag_index_t> index_from_str(std::string_view str);
	static RemoveResult remove(std::string_view tag_name);

	[[nodiscard]] size_t num_files() const;

	// inefficient (O(n) max)
	[[nodiscard]] std::optional<tag_index_t> find_file(std::filesystem::path const& file_name) const;
	[[nodiscard]] bool has_file(std::filesystem::path const& file_name) const {
		return find_file(file_name).has_value();
	}

	// returns the number given to the file
	// ensure_uniqueness can be set to false as an optimization
	tag_index_t tag_file(std::string name_in_all, bool ensure_uniqueness = true) {
		return tag_file(Filesystem::path_all / std::move(name_in_all), ensure_uniqueness);
	}
	tag_index_t tag_file(std::filesystem::path const& file_path, bool ensure_uniqueness = true);

	enum class ReplacementResult : char {
		not_replaced,  // if ensure_uniqueness is true and file was found with this path
		replaced,
	};
	ReplacementResult replace_tagged_file(std::filesystem::path const& file_path, std::filesystem::path const& new_path, bool ensure_uniqueness = true);

	// inefficient (O(n))
	void untag_file(std::filesystem::path const& name_in_all, bool renumber = true);
	void untag_files(Filesystem::FilesSet const& names_in_all, bool renumber = true);
	// efficient (O(1))
	// does not renumber
	void untag_file(tag_index_t index);

	void renumber();

	void rename(std::string_view new_name);

	[[nodiscard]] auto begin() const {
		return TagIterator{ this_tag_path };
	}
	[[nodiscard]] auto end() const {  // NOLINT(readability-convert-member-functions-to-static)
		return TagIterator{};
	}
	[[nodiscard]] bool empty() const {
		return begin() != end();
	}
};

}  // namespace Logic
