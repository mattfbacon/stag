#include <Logic/AllDirectory.hpp>

#include "Filesystem.hpp"
#include "Filesystem/FilesSet.hpp"
#include "Logging.hpp"

namespace Filesystem {

FilesSet::value_type FilesSet::normalize(value_type const& p) {
	auto ret = std::filesystem::absolute(p.lexically_normal());
	Logging::trace("Normalizing path: '{}' -> '{}'", p, ret);
	return ret;
}

std::pair<FilesSet::iterator, bool> FilesSet::insert(value_type const& path) {
	return _impl.insert(normalize(path));
}
FilesSet::iterator FilesSet::insert(decltype(_impl)::const_iterator const& iter, value_type const& path) {
	return _impl.insert(iter, normalize(path));
}
std::pair<FilesSet::iterator, bool> FilesSet::insert_expanded(std::string_view const abbr) {
	return _impl.insert(normalize(Logic::AllDirectory::expand_file_abbreviation(abbr)));
}

size_t FilesSet::size() const {
	return _impl.size();
}
bool FilesSet::empty() const {
	return _impl.empty();
}
FilesSet::iterator FilesSet::begin() {
	return _impl.begin();
}
FilesSet::iterator FilesSet::end() {
	return _impl.end();
}
FilesSet::const_iterator FilesSet::begin() const {
	return _impl.begin();
}
FilesSet::const_iterator FilesSet::end() const {
	return _impl.end();
}
FilesSet::iterator FilesSet::find(value_type const& item) {
	return _impl.find(normalize(item));
}
FilesSet::const_iterator FilesSet::find(value_type const& item) const {
	return _impl.find(normalize(item));
}
bool FilesSet::contains(value_type const& item) const {
	return _impl.contains(normalize(item));
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
	Logging::debug("SLOW: Inverting FilesSet, ending up with {} items", ret.size());
	return ret;
}

FilesSet FilesSet::operator&(FilesSet const& other) const {
	Logging::trace("FilesSet::operator&");
	FilesSet ret;
	for (auto const& item : _impl) {
		if (other._impl.contains(item)) {
			ret._impl.insert(item);
		}
	}
	return ret;
}

FilesSet FilesSet::operator|(FilesSet const& other) const {
	Logging::trace("FilesSet::operator|");
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
	Logging::trace("FilesSet::operator-");
	FilesSet ret;
	for (auto const& item : _impl) {
		if (!other._impl.contains(item)) {
			ret._impl.insert(item);
		}
	}
	return ret;
}

FilesSet::FilesSet(std::initializer_list<value_type> items) {
	for (auto const& item : items) {
		_impl.insert(normalize(item));
	}
}

}  // namespace Filesystem
