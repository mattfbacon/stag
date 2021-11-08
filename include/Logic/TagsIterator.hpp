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
	TagsIterator() : handle{ Filesystem::path_tags }, storage(handle != decltype(handle){} ? value_t::from_path(handle->path()) : value_t{}) {}
	static TagsIterator end() {
		return TagsIterator{ decltype(handle){}, value_t{} };
	}
	bool empty() {
		return handle == decltype(handle){};
	}
	TagsIterator& operator++() {
		handle++;
		// check for end (ADL)
		if (handle != decltype(handle){}) {
			storage = value_t::from_path(handle->path());
		}
		return *this;
	}
	value_t& operator*() {
		return storage;
	}
	value_t* operator->() {
		return &storage;
	}
	bool operator!=(TagsIterator const& other) {
		return handle != other.handle;
	}
};

inline TagsIterator& begin(TagsIterator& x) {
	return x;
}
inline TagsIterator end(TagsIterator const& /* unused */) {
	return TagsIterator::end();
}

}  // namespace Logic
