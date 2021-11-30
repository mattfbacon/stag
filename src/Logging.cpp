#include <magic_enum.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string_view>

#include "Logging.hpp"

#include "common.hpp"

namespace Logging {

std::shared_ptr<spdlog::logger> user_logger;

void set_level_from_string(std::string_view const str) {
	auto* const default_logger = spdlog::default_logger_raw();
	if (auto const enum_log_level = magic_enum::enum_cast<spdlog::level::level_enum>(str)) {
		default_logger->set_level(*enum_log_level);
	} else {
		default_logger->warn("Invalid log level '{}'; using '{}'", str, magic_enum::enum_name(default_logger->level()));
	}
}

namespace {

struct InitLogging {
	InitLogging() {
		auto default_logger = spdlog::stderr_color_mt("default");
		default_logger->set_pattern(STAG_BINARY_NAME ": %l: %v");
		default_logger->flush_on(spdlog::level::err);
		default_logger->enable_backtrace(20);  // NOLINT(readability-magic-numbers)
		default_logger->set_level(spdlog::level::info);
		spdlog::set_default_logger(default_logger);

		user_logger = spdlog::stderr_color_mt("user");
		user_logger->set_pattern(STAG_BINARY_NAME ": user: %v");

		if (char const* env_log_level = std::getenv("STAG_LOG_LEVEL")) {
			set_level_from_string(env_log_level);
		}
	}
	~InitLogging() {
		// https://github.com/gabime/spdlog/wiki/6.-Asynchronous-logging#windows-issues
		spdlog::drop_all();
		spdlog::shutdown();
	}
} _;

}  // namespace

}  // namespace Logging
