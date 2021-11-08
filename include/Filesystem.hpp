#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace Filesystem {

enum class PermsCheck {
	read,
	write,
	exec,
};
void check_perms(std::filesystem::path const& path, PermsCheck which);

void ensure_stagspace();

// extension always either starts with a dot or is empty
// `.jpeg` is converted to `.jpg`
std::string path_extension(std::string_view path);

// returns the path to the imported file
std::filesystem::path import_file(std::filesystem::path const& external_path, bool delete_after_importing);

#define PATH_ALL "all"
#define PATH_TAGS "tags"
#define PATH_VIEWS "views"
#define PATH_DOT_STAGSPACE ".stagspace"
#define PATH_TAGDIR_TO_ALL "../../all"
extern std::filesystem::path const path_all;
extern std::filesystem::path const path_tags;
extern std::filesystem::path const path_views;
extern std::filesystem::path const path_dot_stagspace;
extern std::filesystem::path const path_tagdir_to_all;

}  // namespace Filesystem
