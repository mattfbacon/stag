#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace util {

struct string_equal {
	using is_transparent = std::true_type;
	bool operator()(std::string_view const x, std::string_view const y) const noexcept {
		return x == y;
	}
};

struct string_hash {
	using is_transparent = std::true_type;
	auto operator()(std::string_view str) const noexcept {
		return std::hash<decltype(str)>()(str);
	}
};

template <typename Value>
using string_key_unordered_map = std::unordered_map<std::string, Value, string_hash, string_equal>;
using string_unordered_set = std::unordered_set<std::string, string_hash, string_equal>;

template <typename Container, typename Key, typename Value = typename Container::mapped_type>
std::optional<Value> maybe_get(Container const& container, Key const& key) {
	auto const ret = container.find(key);
	if (ret != container.end()) {
		return std::optional{ ret->second };
	} else {
		return std::nullopt;
	}
}

namespace char_traits {

inline constexpr bool is_upper(char const c) {
	return c >= 'A' && c <= 'Z';
}
inline constexpr bool is_alpha(char const c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
inline constexpr bool is_number(char const c) {
	return c >= '0' && c <= '9';
}
inline constexpr bool is_alnum(char const c) {
	return is_alpha(c) || is_number(c);
}
inline constexpr bool is_tagchar(char const c) {
	return is_alnum(c) || c == ' ';
}

}  // namespace char_traits

struct path_hash {
	std::size_t operator()(std::filesystem::path const& path) const {
		return hash_value(path);
	}
};

}  // namespace util
