#pragma once

#include <cstring>
#include <functional>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "util.hpp"

class ArgParser {
public:
	using arg_t = std::string_view;
	using arg_span_t = std::span<std::string_view>;
	using arg_iter_t = arg_span_t::iterator;
	using long_opt_t = std::string_view;
	using short_opt_t = char;

	enum class OptionCallbackResult {
		no_argument,
		want_argument,
	};

	struct Callbacks {
		using Result = ArgParser::OptionCallbackResult;
		virtual Result short_option_callback(char option_name) = 0;
		virtual Result long_option_callback(std::string_view option_name) = 0;
		// this will only ever be called if the generic function returned Result::want_argument
		virtual void short_option_argument_callback(char option_name, std::string_view value) = 0;
		// this will only ever be called if the generic function returned Result::want_argument
		virtual void long_option_argument_callback(std::string_view option_name, std::string_view value) = 0;
		virtual void argument_callback(std::string_view value) = 0;
	};

	static void parse(Callbacks& callbacks, arg_span_t args);
	static void parse(int argc, char const* const* argv, Callbacks& callbacks);
};
