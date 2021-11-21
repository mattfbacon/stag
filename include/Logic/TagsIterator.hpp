#pragma once

#include <filesystem>
#include <utility>

#include "Filesystem.hpp"
#include "Logic/TagDirectory.hpp"

namespace Logic {

struct TagsIterator {
	using value_t = Logic::TagDirectory;
protected:
	std::filesystem::directory_iterator handle;
	value_t storage;
	TagsIterator(decltype(handle) a_handle, decltype(storage) a_storage) : handle(std::move(a_handle)), storage(std::move(a_storage)) {}
public:
	TagsIterator();
	static TagsIterator end();
	bool empty();
	TagsIterator& operator++();
	value_t& operator*();
	value_t* operator->();
	value_t const& operator*() const;
	value_t const* operator->() const;
	bool operator!=(TagsIterator const& other) const;
};

TagsIterator& begin(TagsIterator& x);
TagsIterator end(TagsIterator const& x);

}  // namespace Logic
