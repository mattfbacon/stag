#pragma once

#include <filesystem>
#include <unordered_set>

#include "util.hpp"

namespace Filesystem {

using FilesAbbrSet = util::string_unordered_set;

struct FilesSet {
	using value_type = std::filesystem::path;
protected:
	std::unordered_set<std::filesystem::path, util::path_hash> _impl;
	[[nodiscard]] static auto normalize(std::filesystem::path const& p) {
		return std::filesystem::absolute(p.lexically_normal());
	}
public:
	auto insert(std::filesystem::path const& path) -> decltype(_impl.insert(path));
	auto insert(decltype(_impl)::const_iterator const& iter, std::filesystem::path const& path) -> decltype(_impl.insert(iter, path));
	auto insert_expanded(std::string_view abbr) -> decltype(_impl.insert(std::declval<std::filesystem::path>()));

	[[nodiscard]] auto size() const {
		return _impl.size();
	}
	[[nodiscard]] auto empty() const {
		return _impl.empty();
	}
	[[nodiscard]] auto begin() {
		return _impl.begin();
	}
	[[nodiscard]] auto end() {
		return _impl.end();
	}
	[[nodiscard]] auto begin() const {
		return _impl.begin();
	}
	[[nodiscard]] auto end() const {
		return _impl.end();
	}
	[[nodiscard]] auto find(std::filesystem::path const& item) const {
		return _impl.find(normalize(item));
	}
	[[nodiscard]] auto contains(std::filesystem::path const& item) const {
		auto const normalized = normalize(item);
		return _impl.contains(normalized);
	}

	[[nodiscard]] FilesSet invert() const;
	[[nodiscard]] FilesSet operator~() const {
		return this->invert();
	}

	[[nodiscard]] FilesSet operator&(FilesSet const& other) const;
	[[nodiscard]] FilesSet operator|(FilesSet const& other) const;
	[[nodiscard]] FilesSet operator-(FilesSet const& other) const;

	FilesSet() = default;
	explicit FilesSet(std::initializer_list<std::filesystem::path> items) {
		for (auto const& item : items) {
			insert(normalize(item));
		}
	}
};

}  // namespace Filesystem
