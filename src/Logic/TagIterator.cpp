#include "Logic/TagIterator.hpp"

namespace Logic {

bool TagIterator::is_tagged_file(std::filesystem::directory_entry const& entry) {
	return entry.is_symlink();
}

TagIterator::TagIterator(std::filesystem::path const& path) : handle(path) {
	auto const end_iter = std::filesystem::end(handle);
	while (handle != end_iter && !is_tagged_file(*handle)) {
		++handle;
	}
}

TagIterator& TagIterator::operator++() {
	auto const end_iter = std::filesystem::end(handle);
	do {
		++handle;
		if (handle == end_iter) {
			break;
		}
	} while (!is_tagged_file(*handle));
	return *this;
}

std::filesystem::path const& TagIterator::operator*() const {
	return handle->path();
}
std::filesystem::path const* TagIterator::operator->() const {
	return &(handle->path());
}
bool TagIterator::operator!=(TagIterator const& other) const {
	return handle != other.handle;
}

}  // namespace Logic
