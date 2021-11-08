#pragma once

#include <string>
#include <utility>

#include "Errors/Base.hpp"
#include "Filesystem/FilesSet.hpp"

namespace Errors::Abbreviation {

struct Base : public ::Errors::Base {};

struct NoMatch : public Base {
	std::string abbreviation;
	Filesystem::FilesSet expansions;
	explicit NoMatch(std::string a_abbreviation) : abbreviation(std::move(a_abbreviation)) {}
	char const* what() const noexcept override {
		return "Errors::Abbreviation::NoMatch";
	}
	void print_to(std::ostream& os) const override;
};

struct Ambiguous : public Base {
	std::string abbreviation;
	Filesystem::FilesSet expansions;
	explicit Ambiguous(std::string a_abbreviation, Filesystem::FilesSet a_expansions)
		: abbreviation(std::move(a_abbreviation)), expansions(std::move(a_expansions)) {}
	char const* what() const noexcept override {
		return "Errors::Abbreviation::Ambiguous";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::Abbreviation
