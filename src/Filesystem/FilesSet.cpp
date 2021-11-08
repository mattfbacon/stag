#include <Logic/AllDirectory.hpp>

#include "Filesystem.hpp"
#include "Filesystem/FilesSet.hpp"

namespace Filesystem {

std::pair<decltype(FilesSet::_impl)::iterator, bool> FilesSet::insert(std::filesystem::path const& path) {
	return _impl.insert(normalize(path));
}
decltype(FilesSet::_impl)::iterator FilesSet::insert(decltype(_impl)::const_iterator const& iter, std::filesystem::path const& path) {
	return _impl.insert(iter, normalize(path));
}
std::pair<decltype(FilesSet::_impl)::iterator, bool> FilesSet::insert_expanded(std::string_view const abbr) {
	return _impl.insert(normalize(Logic::AllDirectory::expand_file_abbreviation(abbr)));
}

FilesSet FilesSet::invert() const {
	FilesSet ret;
	for (auto const& dirent : std::filesystem::directory_iterator{ Filesystem::path_all }) {
		if (!dirent.is_regular_file()) {
			continue;
		}
		auto normalized_path = normalize(dirent.path());
		if (!this->contains(normalized_path)) {
			ret._impl.insert(std::move(normalized_path));
		}
	}
	return ret;
}

FilesSet FilesSet::operator&(FilesSet const& other) const {
	FilesSet ret;
	for (auto const& item : _impl) {
		if (other._impl.contains(item)) {
			ret._impl.insert(item);
		}
	}
	return ret;
}

FilesSet FilesSet::operator|(FilesSet const& other) const {
	FilesSet ret;
	for (auto const& item : _impl) {
		ret._impl.insert(item);
	}
	for (auto const& item : other._impl) {
		ret._impl.insert(item);
	}
	return ret;
}

FilesSet FilesSet::operator-(FilesSet const& other) const {
	FilesSet ret;
	for (auto const& item : _impl) {
		if (!other._impl.contains(item)) {
			ret._impl.insert(item);
		}
	}
	return ret;
}

}  // namespace Filesystem
