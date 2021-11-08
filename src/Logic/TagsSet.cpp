#include <string_view>

#include "Logic/TagsIterator.hpp"
#include "Logic/TagsSet.hpp"

namespace Logic {

void TagsSet::insert_from_string(std::string_view str) {
	// FIXME: when std::ranges work properly, this can be a lot nicer.
	while (!str.empty()) {
		auto const comma_pos = str.find(',');
		auto const substr = str.substr(0, comma_pos);
		if (!substr.empty()) {
			this->emplace(substr);
		}
		if (comma_pos == std::string_view::npos) {
			break;
		}
		str.remove_prefix(comma_pos + 1);
	}
}

TagsSet TagsSet::all() {
	TagsSet ret;
	for (auto const& tag : Logic::TagsIterator{}) {
		ret.insert(tag.name());
	}
	return ret;
}

}  // namespace Logic
