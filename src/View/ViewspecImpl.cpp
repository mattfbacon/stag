#include "Logic/AllDirectory.hpp"
#include "Logic/TagDirectory.hpp"
#include "View/ViewspecImpl.hpp"

namespace View::ViewspecImpl {

Filesystem::FilesSet Tag::evaluate() const {
	Logic::TagDirectory tag{ tag_name };
	Filesystem::FilesSet ret;
	for (auto const& file : tag) {
		ret.insert(tag.dereference(file));
	}
	return ret;
}

Filesystem::FilesSet File::evaluate() const {
	return Filesystem::FilesSet{ file_name };
}

void NotOp::run_children(std::function<void(std::unique_ptr<Base>&)> callback) {
	callback(child);
}
Filesystem::FilesSet NotOp::evaluate() const {
	return ~child->evaluate();
}

void AndOp::run_children(std::function<void(std::unique_ptr<Base>&)> callback) {
	callback(left);
	callback(right);
}
Filesystem::FilesSet AndOp::evaluate() const {
	return left->evaluate() & right->evaluate();
}

void OrOp::run_children(std::function<void(std::unique_ptr<Base>&)> callback) {
	callback(left);
	callback(right);
}
Filesystem::FilesSet OrOp::evaluate() const {
	return left->evaluate() | right->evaluate();
}

void SubtractOp::run_children(std::function<void(std::unique_ptr<Base>&)> callback) {
	callback(minuend);
	callback(subtrahend);
}
Filesystem::FilesSet SubtractOp::evaluate() const {
	return minuend->evaluate() - subtrahend->evaluate();
}

// FIXME: handle precedence and associativity in order to remove parens if not necessary
void Tag::print_to_stream(std::ostream& os) const {
	os << tag_name;
}
void File::print_to_stream(std::ostream& os) const {
	os << file_name.filename();
}
void NotOp::print_to_stream(std::ostream& os) const {
	os << '~';
	os << '(';
	child->print_to_stream(os);
	os << ')';
}
void AndOp::print_to_stream(std::ostream& os) const {
	os << '(';
	left->print_to_stream(os);
	os << ") & (";
	right->print_to_stream(os);
	os << ')';
}
void OrOp::print_to_stream(std::ostream& os) const {
	os << '(';
	left->print_to_stream(os);
	os << ") | (";
	right->print_to_stream(os);
	os << ')';
}
void SubtractOp::print_to_stream(std::ostream& os) const {
	os << '(';
	minuend->print_to_stream(os);
	os << ") - (";
	subtrahend->print_to_stream(os);
	os << ')';
}

}  // namespace View::ViewspecImpl
