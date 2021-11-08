#pragma once

#include <memory>
#include <vector>

#include "Commands/Base.hpp"

namespace Commands {

namespace MetaTagOperation {

struct Base {
	virtual ~Base() = default;
	virtual void take_argument(std::string_view arg) = 0;
	virtual void run() const = 0;
	// throws in case of failure
	virtual void validate() const = 0;
	[[nodiscard]] virtual bool elidable() const {
		return false;
	}
};

}  // namespace MetaTagOperation

struct MetaTag : public Base {
	std::vector<std::unique_ptr<MetaTagOperation::Base>> operations;
	enum class Subcommand {
		create = 'c',
		remove = 'd',
		rename = 'm',
	};
	MetaTag();
	Command what() const override {
		return Command::metatag;
	}
	Result short_option_callback(char option_name) override;
	void short_option_argument_callback(char option_name, std::string_view value) override;
	Result long_option_callback(std::string_view option_name) override;
	void long_option_argument_callback(std::string_view option_name, std::string_view value) override;
	void argument_callback(std::string_view value) override;
	void run() override;
protected:
	void elide_if_possible();
};

}  // namespace Commands
