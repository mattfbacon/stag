#include <cassert>
#include <magic_enum.hpp>
#include <string_view>

#include "Callbacks.hpp"
#include "Commands/MetaTag.hpp"
#include "Errors/ArgParse.hpp"
#include "Errors/Basic.hpp"
#include "Errors/Tags.hpp"
#include "Logging.hpp"
#include "Logic/TagDirectory.hpp"
#include "Logic/TagsSet.hpp"

namespace Commands {

namespace MetaTagOperation {

struct Create : public Base {
protected:
	Logic::TagsSet tags;
public:
	void take_argument(std::string_view const arg) override {
		if (!arg.empty()) {
			tags.emplace(arg);
		}
	}
	void run() const override {
#pragma OMP parallel for
		for (auto const& tag : tags) {
			Logic::TagDirectory{ tag };
		}
	}
	void validate() const override {
		// always valid
	}
	[[nodiscard]] bool elidable() const override {
		return tags.empty();
	}
};

struct Delete : public Base {
protected:
	Logic::TagsSet tags;
public:
	void take_argument(std::string_view const arg) override {
		if (!arg.empty()) {
			tags.emplace(arg);
		}
	}
	void run() const override {
#pragma OMP parallel for
		for (auto const& tag : tags) {
			if (Logic::TagDirectory::remove(tag) == Logic::TagDirectory::RemoveResult::did_not_exist) {
				Logging::warning("Tag queued for deletion does not exist: ", tag);
			}
		}
	}
	void validate() const override {
		// always valid
	}
	[[nodiscard]] bool elidable() const override {
		return tags.empty();
	}
};

struct Rename : public Base {
protected:
	std::string old;
	std::string new_;
	enum class State {
		empty,  // no old, no new
		half,  // have old, no new
		full,  // have both
	} state;
public:
	void take_argument(std::string_view const arg) override {
		switch (state) {
			case State::empty:
				old = arg;
				state = State::half;
				break;
			case State::half:
				new_ = arg;
				state = State::full;
				break;
			case State::full:
				throw Errors::Basic::User{ "Too many arguments to -Rm (tag rename)" };
		}
	}
	void run() const override {
		if (!Logic::TagDirectory::exists(old)) {
			throw Errors::Tags::DoesNotExist{ old };
		}
		Logic::TagDirectory{ old }.rename(new_);
	}
	void validate() const override {
		if (state != State::full) {
			throw Errors::Basic::User{ "Not enough arguments to -Rm (tag rename)" };
		}
		if (old.empty()) {
			throw Errors::Basic::User{ "Empty source given to -Rm (tag rename)" };
		}
		if (new_.empty()) {
			throw Errors::Basic::User{ "Empty destination given to -Rm (tag rename)" };
		}
	}
	[[nodiscard]] bool elidable() const override {
		return old == new_;
	}
};

using Operation = MetaTag::Subcommand;
std::unique_ptr<Base> make(Operation const command) {
	using O = Operation;
	switch (command) {
		case O::create:
			return std::make_unique<Create>();
		case O::rename:
			return std::make_unique<Rename>();
		case O::remove:
			return std::make_unique<Delete>();
	}
	__builtin_unreachable();
}

}  // namespace MetaTagOperation

namespace O = MetaTagOperation;

MetaTag::MetaTag() {
	// create is the default operation
	operations.emplace_back(std::make_unique<O::Create>());
}

void MetaTag::elide_if_possible() {
	if (operations.back()->elidable()) {
		operations.pop_back();
	}
}

Result MetaTag::short_option_callback(char const option_name) {
	if (auto const maybe_subcommand = magic_enum::enum_cast<Subcommand>(option_name); maybe_subcommand.has_value()) {
		elide_if_possible();
		operations.emplace_back(O::make(*maybe_subcommand));
		return Result::no_argument;
	}
	throw Errors::ArgParse::UnknownOption{ option_name };
}

void MetaTag::short_option_argument_callback(char const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

Result MetaTag::long_option_callback(std::string_view const option_name) {
	if (option_name == "create") {
		return short_option_callback('c');
	} else if (option_name == "remove" || option_name == "delete") {
		return short_option_callback('d');
	} else if (option_name == "rename") {
		return short_option_callback('m');
	} else {
		throw Errors::ArgParse::UnknownOption{ option_name };
	}
}

void MetaTag::long_option_argument_callback(std::string_view const option_name, std::string_view value) {
	(void)option_name;
	(void)value;
	assert(false);
}

void MetaTag::argument_callback(std::string_view value) {
	operations.back()->take_argument(value);
}

void MetaTag::run() {
	elide_if_possible();
	for (auto const& operation : operations) {
		operation->validate();
	}
	for (auto const& operation : operations) {
		operation->run();
	}
}

}  // namespace Commands
