#include "Logic/TagsIterator.hpp"

namespace Logic {

TagsIterator::TagsIterator() : handle{ Filesystem::path_tags }, storage(handle != decltype(handle){} ? value_t::from_path(handle->path()) : value_t{}) {}

TagsIterator TagsIterator::end() {
	return TagsIterator{ decltype(handle){}, value_t{} };
}
bool TagsIterator::empty() {
	return handle == decltype(handle){};
}
TagsIterator& TagsIterator::operator++() {
	handle++;
	if (handle != decltype(handle){}) {
		storage = value_t::from_path(handle->path());
	}
	return *this;
}
TagsIterator::value_t& TagsIterator::operator*() {
	return storage;
}
TagsIterator::value_t* TagsIterator::operator->() {
	return &storage;
}
TagsIterator::value_t const& TagsIterator::operator*() const {
	return storage;
}
TagsIterator::value_t const* TagsIterator::operator->() const {
	return &storage;
}
bool TagsIterator::operator!=(TagsIterator const& other) const {
	return handle != other.handle;
}

TagsIterator& begin(TagsIterator& x) {
	return x;
}
TagsIterator end(TagsIterator const& /* unused */) {
	return TagsIterator::end();
}

}  // namespace Logic
