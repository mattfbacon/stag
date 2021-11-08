#pragma once

#include <string>
#include <string_view>

#include "Logic/TagDirectory.hpp"
#include "Logic/TagIterator.hpp"
#include "View/Viewspec.hpp"

namespace View {

class View {
protected:
	std::filesystem::path view_path;
	std::unique_ptr<Viewspec> viewspec;
	bool clean;

	bool exists() const;
	void remove() const;
	void generate_view() const;
public:
	explicit View(std::string_view viewspec, bool clean = true, bool always_generate = false);
	~View();

	[[nodiscard]] size_t num_files() const;
	[[nodiscard]] bool empty() const;

	[[nodiscard]] Logic::TagIterator begin() const;
	[[nodiscard]] Logic::TagIterator end() const;
};

}  // namespace View
