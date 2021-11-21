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
	[[nodiscard]] std::filesystem::path index_to_path(tag_index_t idx) const;
	[[nodiscard]] std::filesystem::path current_path() const;
	TagDirectory(std::filesystem::path a_this_tag_path, std::string a_tag_name);
public:
	enum class RemoveResult {
		removed,
		did_not_exist,
	};

	TagDirectory() = default;
	[[nodiscard]] bool valid() const;
	explicit TagDirectory(std::string_view a_tag_name) : TagDirectory{ std::string{ a_tag_name } } {}
	explicit TagDirectory(std::string&& a_tag_name);
	[[nodiscard]] static TagDirectory from_path(std::filesystem::path actual_directory);
	[[nodiscard]] static bool exists(std::string_view tag_name);

	[[nodiscard]] std::filesystem::path dereference(std::filesystem::path const& file) const;

	[[nodiscard]] std::string const& name() const;

	[[nodiscard]] static std::optional<tag_index_t> index_from_str(std::string_view str);
	static RemoveResult remove(std::string_view tag_name);

	[[nodiscard]] size_t num_files() const;

	// inefficient (O(n) max)
	[[nodiscard]] std::optional<tag_index_t> find_file(std::filesystem::path const& file_name) const;
	[[nodiscard]] bool has_file(std::filesystem::path const& file_name) const;

	// returns the number given to the file
	// ensure_uniqueness can be set to false as an optimization
	tag_index_t tag_file(std::string name_in_all, bool ensure_uniqueness = true);
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
	void strip_broken();  // also renumbers in the process

	void rename(std::string_view new_name);

	[[nodiscard]] TagIterator begin() const;
	[[nodiscard]] TagIterator end() const;
	[[nodiscard]] bool empty() const;
	[[nodiscard]] std::optional<std::filesystem::path> first_file() const;
};

}  // namespace Logic
