#pragma once

#include <filesystem>

namespace View {
class View;
}

namespace Logic {

using tag_index_t = size_t;

class TagDirectory;

class TagIterator {
protected:
	std::filesystem::path item_path;
	tag_index_t current_number = 0;  // can be incremented multiple times in case of gaps
	tag_index_t current_index = 0;  // always incremented once
	tag_index_t num_tagged_files = 0;
	TagIterator();  // default-constructed = end iter
	explicit TagIterator(std::filesystem::path dir_path);
	bool is_at_end() const;
public:
	TagIterator& operator++();
	std::filesystem::path const& operator*() const;
	std::filesystem::path const* operator->() const;
	bool operator!=(TagIterator const& other) const;
	friend class TagDirectory;
	friend class View::View;
};

}  // namespace Logic
