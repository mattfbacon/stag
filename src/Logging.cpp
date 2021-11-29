#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Logging.hpp"

#include "common.hpp"

namespace Logging {

std::shared_ptr<spdlog::logger> user_logger;

namespace {

struct InitLogging {
	InitLogging() {
		spdlog::cfg::load_env_levels();

		auto default_logger = spdlog::stderr_color_mt("default");
		default_logger->set_pattern(STAG_BINARY_NAME ": %l: %v");
		default_logger->flush_on(spdlog::level::err);
		default_logger->enable_backtrace(20);  // NOLINT(readability-magic-numbers)
		spdlog::set_default_logger(default_logger);

		user_logger = spdlog::stderr_color_mt("user");
		user_logger->set_pattern(STAG_BINARY_NAME ": user: %v");
	}
	~InitLogging() {
		// https://github.com/gabime/spdlog/wiki/6.-Asynchronous-logging#windows-issues
		spdlog::drop_all();
		spdlog::shutdown();
	}
} _;

}  // namespace

}  // namespace Logging
