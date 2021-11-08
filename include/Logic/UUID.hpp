#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "BinaryView.hpp"

namespace Logic::UUID {

std::string from_file(std::filesystem::path const& filename);
std::string from_data(BinaryView);

}  // namespace Logic::UUID
