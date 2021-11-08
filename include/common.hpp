#pragma once

#include <cstddef>

#define STAG_XSTR(STR) STAG_STR(STR)
#define STAG_STR(STR) #STR

#define STAG_VERSION_MAJOR 0
#define STAG_VERSION_MINOR 1
#define STAG_VERSION_PATCH 0
#define STAG_PROGRAM_VERSION STAG_XSTR(STAG_VERSION_MAJOR) "." STAG_XSTR(STAG_VERSION_MINOR) "." STAG_XSTR(STAG_VERSION_PATCH)
#define STAG_BINARY_NAME "stag"
#define STAG_PROGRAM_NAME "STag"

// will result in 40 hex chars
constexpr size_t HASH_NUM_BYTES = 20;
