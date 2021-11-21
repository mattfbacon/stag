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
	static bool is_tagged_file(std::filesystem::directory_entry const& entry);
	TagIterator() = default;  // default-constructed = end iter
	explicit TagIterator(std::filesystem::path const& path);
public:
	TagIterator& operator++();
	std::filesystem::path const& operator*() const;
	std::filesystem::path const* operator->() const;
	bool operator!=(TagIterator const& other) const;
	friend class TagDirectory;
	friend class View::View;
};

}  // namespace Logic
