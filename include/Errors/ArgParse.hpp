#pragma once

#include <string>

#include "ArgParser.hpp"
#include "Errors/Base.hpp"

namespace Errors::ArgParse {

struct Base : public ::Errors::Base {};

enum class OptType {
	short_opt,
	long_opt,
};
struct UnknownOption : public Base {
	OptType opt_type;
	std::string opt_name;
	explicit UnknownOption(ArgParser::long_opt_t const a_long_opt_name) : opt_type(OptType::long_opt), opt_name{ a_long_opt_name } {}
	explicit UnknownOption(ArgParser::short_opt_t const a_short_opt_name) : opt_type(OptType::short_opt), opt_name{ a_short_opt_name } {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::ArgParser::UnknownOption";
	}
	void print_to(std::ostream& os) const override;
};

struct ArgumentMismatch : public Base {
	enum class Reason {
		provided_not_expected,
		expected_not_provided,
	};
	Reason what_happened;
	OptType opt_type;
	std::string opt_name;
	explicit ArgumentMismatch(Reason a_what_happened, ArgParser::long_opt_t const a_long_opt_name)
		: what_happened(a_what_happened), opt_type(OptType::long_opt), opt_name{ a_long_opt_name } {}
	explicit ArgumentMismatch(Reason a_what_happened, ArgParser::short_opt_t const a_short_opt_name)
		: what_happened(a_what_happened), opt_type(OptType::short_opt), opt_name{ a_short_opt_name } {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::ArgParse::ArgumentMismatch";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::ArgParse
