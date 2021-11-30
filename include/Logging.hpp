#pragma once

#include <filesystem>
#include <fmt/ostream.h>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <tao/pegtl/position.hpp>

namespace Logging {

extern std::shared_ptr<spdlog::logger> user_logger;

void set_level_from_string(std::string_view str);

using spdlog::trace, spdlog::debug, spdlog::info, spdlog::warn, spdlog::error, spdlog::critical;

template <typename... Args>
constexpr auto user(spdlog::format_string_t<Args...> fmt, Args&&... args) {
	return user_logger->info(fmt, std::forward<Args>(args)...);
}

}  // namespace Logging
