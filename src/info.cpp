#include <cryptopp/config_ver.h>
#include <iostream>
#include <magic_enum.hpp>
#include <ostream>
#include <tao/pegtl/version.hpp>

#include "common.hpp"
#include "info.hpp"

void print_usage_to(std::ostream& os) {
	// clang-format off
	os <<
		"Usage: `" STAG_BINARY_NAME " {-UQTRFVO} [options] [...targets]`\n"
		"For more information, refer to the manual page.\n"
		;
	// clang-format on
}

void print_version() {
	// clang-format off
	std::cout <<
		STAG_PROGRAM_NAME " version " STAG_PROGRAM_VERSION "\n"
		"Made by Matt Fellenz.\n"
		"Licensed under the AGPL 3.0 or later\n"
		"Submodule versions:\n"
		"  PEGTL version " TAO_PEGTL_VERSION " (https://github.com/taocpp/pegtl)\n"
		"  Magic Enum version " STAG_XSTR(MAGIC_ENUM_VERSION_MAJOR) "." STAG_XSTR(MAGIC_ENUM_VERSION_MINOR) "." STAG_XSTR(MAGIC_ENUM_VERSION_PATCH) " (https://github.com/Neargye/magic_enum)\n"
		"  CryptoPP version " STAG_XSTR(CRYPTOPP_MAJOR) "." STAG_XSTR(CRYPTOPP_MINOR) "." STAG_XSTR(CRYPTOPP_REVISION) " (https://github.com/weidai11/cryptopp)\n"
		;
	// clang-format on
}
