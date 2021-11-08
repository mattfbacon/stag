#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <ostream>
#include <string>

#include "Filesystem/FilesSet.hpp"

namespace View::ViewspecImpl {

struct Base {
	virtual Filesystem::FilesSet evaluate() const = 0;
	virtual void run_children(std::function<void(std::unique_ptr<Base>&)> callback) = 0;
	virtual void print_to_stream(std::ostream& os) const = 0;
	virtual int precedence() const = 0;
	virtual ~Base() = default;
};

struct Tag final : public Base {
	std::string tag_name;
	Tag(std::string a_tag_name) : tag_name(std::move(a_tag_name)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> /* unused */) override {}
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 0;
	}
	Filesystem::FilesSet evaluate() const override;
};

struct File final : public Base {
	std::filesystem::path file_name;
	File(std::filesystem::path a_file_name) : file_name(std::move(a_file_name)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> /* unused */) override {}
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 1;
	}
	Filesystem::FilesSet evaluate() const override;
};

// This operation is not great. The resulting set can be very large.
struct NotOp final : public Base {
	std::unique_ptr<Base> child;
	NotOp(std::unique_ptr<Base>&& a_child) : child(std::move(a_child)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> callback) override;
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 90;
	}
	Filesystem::FilesSet evaluate() const override;
};

struct AndOp final : public Base {
	std::unique_ptr<Base> left, right;
	AndOp(std::unique_ptr<Base>&& a_left, std::unique_ptr<Base>&& a_right) : left(std::move(a_left)), right(std::move(a_right)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> callback) override;
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 70;
	}
	Filesystem::FilesSet evaluate() const override;
};

struct OrOp final : public Base {
	std::unique_ptr<Base> left, right;
	OrOp(std::unique_ptr<Base>&& a_left, std::unique_ptr<Base>&& a_right) : left(std::move(a_left)), right(std::move(a_right)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> callback) override;
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 60;
	}
	Filesystem::FilesSet evaluate() const override;
};

struct SubtractOp final : public Base {
	std::unique_ptr<Base> minuend, subtrahend;  // minuend - subtrahend = difference
	SubtractOp(std::unique_ptr<Base>&& a_left, std::unique_ptr<Base>&& a_right) : minuend(std::move(a_left)), subtrahend(std::move(a_right)) {}
	void run_children(std::function<void(std::unique_ptr<Base>&)> callback) override;
	void print_to_stream(std::ostream& os) const override;
	int precedence() const override {
		return 80;
	}
	Filesystem::FilesSet evaluate() const override;
};

inline std::ostream& operator<<(std::ostream& os, Base const& v) {
	v.print_to_stream(os);
	return os;
}

}  // namespace View::ViewspecImpl
