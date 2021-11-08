#pragma once

#include <filesystem>
#include <utility>

#include "Errors/Base.hpp"
#include "Filesystem.hpp"

namespace Errors::OS {

struct Base : public ::Errors::Base {};

struct Permissions : Base {
	std::filesystem::path path;
	Filesystem::PermsCheck permission;

	Permissions(std::filesystem::path a_path, Filesystem::PermsCheck a_permission) : path(std::move(a_path)), permission(a_permission) {}
	[[nodiscard]] char const* what() const noexcept override {
		return "Errors::OS::Permissions";
	}
	void print_to(std::ostream& os) const override;
};

}  // namespace Errors::OS
