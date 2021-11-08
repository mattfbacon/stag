#include "Filesystem.hpp"
#include "Logging.hpp"
#include "Logic/TagDirectory.hpp"
#include "View/Parser.hpp"
#include "View/View.hpp"

namespace View {

void View::generate_view() const {
	auto view_dir = Logic::TagDirectory::from_path(view_path);
	for (auto const& item : viewspec->evaluate()) {
		view_dir.tag_file(item, false);
	}
}

bool View::exists() const {
	return std::filesystem::exists(view_path);
}

void View::remove() const {
	std::filesystem::remove_all(view_path);
}

View::View(std::string_view const a_viewspec, bool const a_clean, bool const always_generate) : viewspec(Parser::parse(a_viewspec)), clean(a_clean) {
	std::ostringstream viewspec_repr_builder;
	viewspec->print_to_stream(viewspec_repr_builder);
	view_path = Filesystem::path_views / std::move(viewspec_repr_builder).str();
	if (exists()) {
		auto const view_mtime = std::filesystem::last_write_time(view_path);
		auto const tags_mtime = std::filesystem::last_write_time(Filesystem::path_tags);
		if (always_generate || tags_mtime > view_mtime) {
			remove();
			generate_view();
		} else {
			Logging::info("Using existing view for ", view_path.filename());
		}
	} else {
		generate_view();
	}
}

View::~View() {
	if (clean && exists()) {
		remove();
	}
}

size_t View::num_files() const {
	size_t ret = 0;
	for (auto const& dirent : std::filesystem::directory_iterator{ view_path }) {
		if (!dirent.is_symlink()) {
			continue;
		}
		++ret;
	}
	return ret;
}

bool View::empty() const {
	for (auto const& dirent : std::filesystem::directory_iterator{ view_path }) {
		if (!dirent.is_symlink()) {
			continue;
		}
		return false;
	}
	return true;
}

Logic::TagIterator View::begin() const {
	return Logic::TagIterator{ view_path };
}

Logic::TagIterator View::end() const {
	return Logic::TagIterator{};
}

}  // namespace View
