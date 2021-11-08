#pragma once

#include <string>
#include <unordered_set>

namespace Logic {

struct TagsSet : public std::unordered_set<std::string> {
	template <typename... Args>
	explicit TagsSet(Args... args) : std::unordered_set<std::string>{ args... } {}

	// "tag1,tag2,tag3" -> insert tag1, tag2, and tag3 into the set
	void insert_from_string(std::string_view);
	static TagsSet all();
};

}  // namespace Logic
