#include <algorithm>
#include <fstream>
#include <unistd.h>

#include "Errors/Basic.hpp"
#include "Errors/OS.hpp"
#include "Filesystem.hpp"
#include "Logging.hpp"
#include "Logic/UUID.hpp"

#include "common.hpp"

namespace Filesystem {

std::filesystem::path const path_all{ PATH_ALL };
std::filesystem::path const path_tags{ PATH_TAGS };
std::filesystem::path const path_views{ PATH_VIEWS };
std::filesystem::path const path_dot_stagspace{ PATH_DOT_STAGSPACE };
std::filesystem::path const path_tagdir_to_all{ PATH_TAGDIR_TO_ALL };

/**
 * If the current directory has .stagspace, we consider it ours to modify.
 * If it doesn't have that, the existence of any other non-dotfile means that data might be overwritten (somehow), so we want to warn the user.
 */
void ensure_stagspace() {
	Logging::trace("Ensuring the existence of a " STAG_PROGRAM_NAME "-space");
	if (!std::filesystem::exists(Filesystem::path_dot_stagspace)) {
		Logging::trace("  " PATH_DOT_STAGSPACE " does not exist");
		auto dir_it = std::filesystem::directory_iterator{ std::filesystem::current_path() };
		// clang-format off
		if (!std::all_of(
			begin(dir_it),
			end(dir_it),
			[](std::filesystem::directory_entry const& file) {
				return file.path().filename().string().starts_with(".");  // matches any dotfile
			}
		)) {
			throw Errors::Basic::User{ "Current directory has contents but is not marked as a " STAG_PROGRAM_NAME "-space. To resolve this issue, either create a file called '" PATH_DOT_STAGSPACE "', or remove the current contents." };
			// clang-format on
		}
		std::ofstream{ Filesystem::path_dot_stagspace };
	}
	Logging::trace("  Creating " STAG_PROGRAM_NAME "-space directories");
#pragma OMP parallel for
	for (auto const& path : { Filesystem::path_all, Filesystem::path_tags, Filesystem::path_views }) {
		std::filesystem::create_directories(path);
	}
}

std::filesystem::path import_file(std::filesystem::path const& external_path, bool const delete_after_importing) {
	Logging::trace("Importing '{}' into the All directory (delete after importing? {})", external_path, delete_after_importing);
	auto imported_filename = Filesystem::path_all / Logic::UUID::from_file(external_path.c_str());
	imported_filename.replace_extension(external_path.extension());
	if (delete_after_importing) {
		std::filesystem::rename(external_path, imported_filename);
	} else {
		std::filesystem::copy(external_path, imported_filename);
	}
	return imported_filename;
}

}  // namespace Filesystem

#if defined(linux) || defined(__linux__)

namespace {
[[noreturn]] inline void throw_errno() {
	throw std::system_error{ errno, std::system_category() };
}
}  // namespace

namespace Filesystem {

void check_perms(std::filesystem::path const& path, PermsCheck const which) {
	int translated_which = 0;
	switch (which) {
		case PermsCheck::read:
			translated_which = R_OK;
			break;
		case PermsCheck::write:
			translated_which = W_OK;
			break;
		case PermsCheck::exec:
			translated_which = X_OK;
			break;
	}
	if (eaccess(path.c_str(), translated_which) < 0) {
		if (errno != EACCES) {
			throw_errno();
		}
		throw Errors::OS::Permissions{ path, which };
	}
}

}  // namespace Filesystem

#else

namespace Filesystem {

void check_perms(std::filesystem::path const&, PermsCheck) {
#warning "check_perms not implemented for this platform; it will never fail"
}

}  // namespace Filesystem

#endif
