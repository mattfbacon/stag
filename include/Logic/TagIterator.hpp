#pragma once

#include <filesystem>

namespace View {
class View;
}

namespace Logic {

class TagDirectory;

class TagIterator {
protected:
	std::filesystem::directory_iterator handle;
	static bool is_tagged_file(std::filesystem::directory_entry const& entry) {
		return entry.is_symlink();
	}
	TagIterator() = default;  // default-constructed = end iter
	explicit TagIterator(std::filesystem::path const& path) : handle(path) {
		auto const end_iter = std::filesystem::end(handle);
		while (handle != end_iter && !is_tagged_file(*handle)) {
			++handle;
		}
	}
public:
	TagIterator& operator++() {
		auto const end_iter = std::filesystem::end(handle);
		do {
			++handle;
			if (handle == end_iter) {
				break;
			}
		} while (!is_tagged_file(*handle));
		return *this;
	}
	std::filesystem::path const& operator*() const {
		return handle->path();
	}
	std::filesystem::path const* operator->() const {
		return &(handle->path());
	}
	bool operator!=(TagIterator const& other) const {
		return handle != other.handle;
	}
	friend class TagDirectory;
	friend class View::View;
};

}  // namespace Logic
