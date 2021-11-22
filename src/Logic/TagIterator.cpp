#include "Logic/TagIterator.hpp"

#include "util.hpp"

namespace Logic {

TagIterator::TagIterator() {
	current_index = 1;
	num_tagged_files = 0;
	// is_at_end() will now return true
}

TagIterator::TagIterator(std::filesystem::path a_path) : item_path(std::move(a_path)) {
	for (auto const& dirent : std::filesystem::directory_iterator{ item_path }) {
		auto const dirent_name = dirent.path().filename().string();
		if (dirent.is_symlink() && std::all_of(dirent_name.begin(), dirent_name.end(), util::char_traits::is_number)) {
			++num_tagged_files;
		}
	}
	item_path /= std::to_string(current_number);
	this->operator++();
}

bool TagIterator::is_at_end() const {
	return current_index > num_tagged_files;
}

TagIterator& TagIterator::operator++() {
	++current_index;
	if (!is_at_end()) {
		do {
			++current_number;
			item_path.replace_filename(std::to_string(current_number));
		} while (!std::filesystem::exists(item_path));
	}
	return *this;
}

std::filesystem::path const& TagIterator::operator*() const {
	return item_path;
}
std::filesystem::path const* TagIterator::operator->() const {
	return &item_path;
}
std::filesystem::path& TagIterator::operator*() {
	return item_path;
}
std::filesystem::path* TagIterator::operator->() {
	return &item_path;
}
bool TagIterator::operator==(TagIterator const& other) const {
	return (is_at_end() && other.is_at_end()) || (item_path == other.item_path);
}

TagIterator& begin(TagIterator& x) {
	return x;
}
TagIterator end(TagIterator const&) {
	return TagIterator{};
}

}  // namespace Logic
