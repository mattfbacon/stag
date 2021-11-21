#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_set>
#include <utility>  // pair

#include "util.hpp"

namespace Filesystem {

using FilesAbbrSet = util::string_unordered_set;

struct FilesSet {
	using value_type = std::filesystem::path;
protected:
	std::unordered_set<value_type, util::path_hash> _impl;
	[[nodiscard]] static value_type normalize(value_type const& p);
public:
	using iterator = decltype(_impl)::iterator;
	using const_iterator = decltype(_impl)::const_iterator;

	std::pair<iterator, bool> insert(value_type const& path);
	iterator insert(const_iterator const& iter, value_type const& path);
	std::pair<iterator, bool> insert_expanded(std::string_view abbr);

	[[nodiscard]] size_t size() const;
	[[nodiscard]] bool empty() const;
	[[nodiscard]] iterator begin();
	[[nodiscard]] iterator end();
	[[nodiscard]] const_iterator begin() const;
	[[nodiscard]] const_iterator end() const;
	[[nodiscard]] iterator find(value_type const& item);
	[[nodiscard]] const_iterator find(value_type const& item) const;
	[[nodiscard]] bool contains(value_type const& item) const;

	[[nodiscard]] FilesSet invert() const;
	[[nodiscard]] FilesSet operator~() const {
		return this->invert();
	}

	[[nodiscard]] FilesSet operator&(FilesSet const& other) const;
	[[nodiscard]] FilesSet operator|(FilesSet const& other) const;
	[[nodiscard]] FilesSet operator-(FilesSet const& other) const;

	FilesSet() = default;
	explicit FilesSet(std::initializer_list<value_type> items);
};

}  // namespace Filesystem
