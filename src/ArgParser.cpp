#include <cassert>

#include "ArgParser.hpp"
#include "Errors/ArgParse.hpp"
#include "Logging.hpp"

#include "common.hpp"

using arg_t = ArgParser::arg_t;
using arg_span_t = ArgParser::arg_span_t;
using arg_iter_t = ArgParser::arg_iter_t;
using Callbacks = ArgParser::Callbacks;
using OptionCallbackResult = ArgParser::OptionCallbackResult;
namespace ArgErr = Errors::ArgParse;
using ArgumentMismatchReason = ArgErr::ArgumentMismatch::Reason;

enum class ArgType {
	positional,  // anything not matching anything else
	short_option,  // e.g., `-a`, `-ab`
	long_option,  // e.g., `--long`
	end_options,  // i.e., `--`
};

template <typename Iter>
bool safe_iter_increment(Iter& iter, Iter const& iter_end) {
	++iter;
	return iter < iter_end;
}

ArgType identify_arg_type(std::string_view const arg) {
	if (!arg.starts_with("-")) {
		return ArgType::positional;
	}
	if (arg.size() == 1) {  // `-`
		return ArgType::positional;
	}
	if (arg[1] == '-') {  // `--*`
		if (arg.size() == 2) {  // `--`
			return ArgType::end_options;
		} else {  // `--?*`
			return ArgType::long_option;
		}
	}
	return ArgType::short_option;  // `-?*`
}

bool check_next_argument(arg_iter_t& arg_iter, arg_iter_t const& arg_end_iter) {
	Logging::trace("ArgParser: checking for next argument");
	if (!safe_iter_increment(arg_iter, arg_end_iter)) {
		return false;
	}
	return identify_arg_type(*arg_iter) == ArgType::positional;
}

void parse_short_option_block(Callbacks& callbacks, arg_t short_option_block, arg_iter_t& arg_iter, arg_iter_t const& arg_end_iter) {
	Logging::trace("ArgParser: parsing short option block with content '{}'", short_option_block);
	do {
		auto const short_option_name = short_option_block[0];
		auto const option_callback_result = callbacks.short_option_callback(short_option_name);
		switch (option_callback_result) {
			case OptionCallbackResult::want_argument: {
				std::string_view option_argument = short_option_block.substr(1);
				if (option_argument.empty()) {  // `-b org` instead of `-borg`
					if (check_next_argument(arg_iter, arg_end_iter)) {
						option_argument = *arg_iter;
					} else {
						throw Errors::ArgParse::ArgumentMismatch{ ArgumentMismatchReason::expected_not_provided, short_option_name };
					}
				}
				callbacks.short_option_argument_callback(short_option_name, option_argument);
				return;
			}
			case OptionCallbackResult::no_argument:
				break;
		}
		short_option_block.remove_prefix(1);
	} while (!short_option_block.empty());
}

void parse_long_option(Callbacks& callbacks, arg_t long_option_content, arg_iter_t& arg_iter, arg_iter_t const& arg_end_iter) {
	Logging::trace("ArgParser: parsing long option with content '{}'", long_option_content);
	auto const equals_sign_position = long_option_content.find('=');
	auto const long_option_name = long_option_content.substr(0, equals_sign_position);
	auto const option_callback_result = callbacks.long_option_callback(long_option_name);
	switch (option_callback_result) {
		case OptionCallbackResult::want_argument: {
			std::string_view option_argument;
			if (equals_sign_position != decltype(long_option_content)::npos) {
				option_argument = long_option_content.substr(equals_sign_position + 1);
			} else {
				if (check_next_argument(arg_iter, arg_end_iter)) {
					option_argument = *arg_iter;
				} else {
					throw ArgErr::ArgumentMismatch{ ArgumentMismatchReason::expected_not_provided, long_option_name };
				}
			}
			callbacks.long_option_argument_callback(long_option_name, option_argument);
			break;
		}
		case OptionCallbackResult::no_argument:
			if (equals_sign_position != decltype(long_option_content)::npos) {  // argument provided to option that takes no arguments
				throw ArgErr::ArgumentMismatch{ ArgumentMismatchReason::provided_not_expected, long_option_name };
			}
			break;
	}
}

void ArgParser::parse(Callbacks& callbacks, arg_span_t const args) {
	arg_iter_t arg_iter = args.begin();
	arg_iter_t const arg_end_iter = args.end();
	for (; arg_iter < arg_end_iter; ++arg_iter) {
		arg_t const arg = *arg_iter;
		Logging::trace("ArgParser: raw argument is '{}'", arg);
		switch (identify_arg_type(arg)) {
			case ArgType::positional:
				callbacks.argument_callback(arg);
				break;
			case ArgType::short_option:
				parse_short_option_block(callbacks, arg.substr(1), arg_iter, arg_end_iter);
				break;
			case ArgType::long_option:
				parse_long_option(callbacks, arg.substr(2), arg_iter, arg_end_iter);
				break;
			case ArgType::end_options:
				goto exit_loop;
		}
		continue;
exit_loop:;
		++arg_iter;
		break;
	}
	// if the previous loop finished all the arguments, this one won't run.
	// the only case where this will run is if -- is encountered.
	for (; arg_iter < arg_end_iter; ++arg_iter) {
		callbacks.argument_callback(*arg_iter);
	}
}

void ArgParser::parse(int const argc, char const* const* const argv, Callbacks& callbacks) {
	Logging::trace("ArgParser: parser invoked with argc and argv; converting to vector<string_view>");
	std::vector<std::string_view> args;
	assert(argc > 0);
	if (argc == 0) {
		return;
	}
	args.resize(static_cast<size_t>(argc - 1));
	// skip argv[0]
	std::transform(argv + 1, argv + argc, args.begin(), [](char const* arg) {
		return std::string_view{ arg, std::strlen(arg) };
	});
	return parse(callbacks, args);
}
