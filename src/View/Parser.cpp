#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

#include "Errors/Viewspec.hpp"
#include "View/Parser.hpp"

#if !defined(NDEBUG)
#include <cassert>
#include <tao/pegtl/contrib/analyze.hpp>
#endif

namespace View::Parser {

namespace P = tao::pegtl;

namespace Exprs {

struct _identifier : P::list<P::plus<P::ascii::alnum>, P::ascii::space> {};
// can't start or end with a space
struct tag : _identifier {};

struct file : P::seq<_identifier, P::one<'.'>, P::opt<_identifier>> {};

template <char... C>
using operator_ = P::pad<P::one<C...>, P::space>;
struct operator_not : operator_<'~', '!'> {};
struct operator_and : operator_<'&'> {};
struct operator_or : operator_<'|'> {};
struct operator_add : operator_<'+'> {};
struct operator_subtract : operator_<'-'> {};

struct open_bracket : operator_<'('> {};
struct close_bracket : operator_<')'> {};

struct prec_layer_0;
struct prec_layer_4;
struct bracketed : P::if_must<open_bracket, prec_layer_0, close_bracket> {};
struct not_expr : P::if_must<operator_not, prec_layer_4> {};

template <typename Child, typename... Operators>
struct left_assoc_operator : P::list<Child, P::sor<Operators...>, P::space> {};
template <typename Child, typename... Operators>
struct right_assoc_operator : P::seq<P::pad<Child, P::space>, P::opt<P::seq<P::sor<Operators...>, right_assoc_operator<Child, Operators...>>>> {};

struct prec_layer_4 : P::sor<bracketed, file, tag> {};
struct prec_layer_3 : P::sor<not_expr, prec_layer_4> {};
struct prec_layer_2 : left_assoc_operator<prec_layer_3, operator_add, operator_subtract> {};
struct prec_layer_1 : left_assoc_operator<prec_layer_2, operator_and> {};
struct prec_layer_0 : left_assoc_operator<prec_layer_1, operator_or> {};

struct grammar : P::must<P::pad<prec_layer_0, P::space>, P::eof> {};

template <bool RightAssoc = false>
struct operator_rearrange : P::parse_tree::apply<operator_rearrange<RightAssoc>> {
	template <typename... States>
	static void transform(std::unique_ptr<P::parse_tree::node>& node, States&&... st) {
		if (node->children.size() == 1) {
			node = std::move(node->children[0]);
		} else {
			assert(node->children.size() > 1 && node->children.size() % 2 == 1);
			node->remove_content();
			auto& children = node->children;
			auto& first_operand = node;
			auto second_operand = std::move(children.back());
			children.pop_back();
			auto operator_ = std::move(children.back());
			children.pop_back();
			operator_->children.emplace_back(std::move(first_operand));
			operator_->children.emplace_back(std::move(second_operand));
			node = std::move(operator_);
			auto& recurse_child = node->children.front();
			if (recurse_child->is_type<prec_layer_0>() || recurse_child->is_type<prec_layer_1>() || recurse_child->is_type<prec_layer_2>()) {
				transform(recurse_child, st...);
			}
		}
	}
};
template <>
struct operator_rearrange<true> : P::parse_tree::apply<operator_rearrange<true>> {
	template <typename... States>
	static void transform(std::unique_ptr<P::parse_tree::node>& node, States&&... st) {
		if (node->children.size() == 1) {
			node = std::move(node->children[0]);
		} else {
			assert(node->children.size() > 1 && node->children.size() % 2 == 1);
			node->remove_content();
			auto& children = node->children;
			auto first_operand = std::move(children[0]);
			auto& second_operand = node;
			auto operator_ = std::move(children[1]);
			operator_->children.emplace_back(std::move(first_operand));
			// take the rest of the children, starting at the third (index 2), since the first two have already been "stolen"
			second_operand->children = std::vector(std::make_move_iterator(children.begin() + 2), std::make_move_iterator(children.end()));
			operator_->children.emplace_back(std::move(second_operand));
			node = std::move(operator_);
			auto& recurse_child = node->children.back();
			if (recurse_child->is_type<prec_layer_0>() || recurse_child->is_type<prec_layer_1>() || recurse_child->is_type<prec_layer_2>()) {
				transform(recurse_child, st...);
			}
		}
	}
};

struct operator_as_node : P::parse_tree::apply<operator_as_node> {
	template <typename... States>
	static void transform(std::unique_ptr<P::parse_tree::node>& node, States&&...) {
		// transforms from...
		//          operator_wrapper
		//  operator_character  arg...
		// ...to...
		//  operator_character
		//     arg...

		// first remove any content from the node
		node->remove_content();
		auto& children = node->children;
		assert(children.size() >= 1);
		// take the name and store it for later
		auto operator_character_node = std::move(children.front());
		children.erase(children.begin());  // children is now just the arguments
		using std::swap;
		// make the arguments the children of the operator wrapper
		swap(operator_character_node->children, node->children);
		// and finally make the node be the operator character
		node = std::move(operator_character_node);
	}
};

// clang-format off
template <typename Rule>
using selector = P::parse_tree::selector<
	Rule
	, P::parse_tree::store_content::on<tag, file>
	, P::parse_tree::remove_content::on<operator_not, operator_and, operator_or, operator_add, operator_subtract>
	, operator_as_node::on<not_expr>
	, operator_rearrange<false>::on<prec_layer_0, prec_layer_1, prec_layer_2>
	>;
// clang-format on

}  // namespace Exprs

namespace SyntaxTree {

#define MAPPING(SOURCE, CTOR) \
	if (root->is_type<Exprs::SOURCE>()) { \
		return (CTOR); \
	}
#define BINARY_MAPPING(SOURCE, DEST) \
	MAPPING(SOURCE, std::make_unique<ViewspecImpl::DEST>(make_syntax_tree_helper(root->children[0]), make_syntax_tree_helper(root->children[1])))
#define UNARY_MAPPING(SOURCE, DEST) MAPPING(SOURCE, std::make_unique<ViewspecImpl::DEST>(make_syntax_tree_helper(root->children[0])))
#define DIRECT_MAPPING(SOURCE, DEST) MAPPING(SOURCE, std::make_unique<ViewspecImpl::DEST>(root->string()));

std::unique_ptr<Viewspec> make_syntax_tree_helper(std::unique_ptr<tao::pegtl::parse_tree::node> const& root) {
	DIRECT_MAPPING(tag, Tag);
	DIRECT_MAPPING(file, File);
	UNARY_MAPPING(operator_not, NotOp);
	BINARY_MAPPING(operator_and, AndOp);
	BINARY_MAPPING(operator_or, OrOp);
	BINARY_MAPPING(operator_add, OrOp);
	BINARY_MAPPING(operator_subtract, SubtractOp);
	throw std::runtime_error{ "Missing mapping for node " + std::string{ root->type } };
}

#undef BINARY_MAPPING
#undef UNARY_MAPPING
#undef DIRECT_MAPPING
#undef MAPPING

/**
 * The goal of this function is to remove usage of the not operator.
 * The not operator is very inefficient because it returns a very large set.
 * We want to reduce the usage of this operator, and if possible to move it up the tree, so more optimizations can be done higher up the tree.
 * These optimizations can be done recursively so we do it with DFS, starting with the children.
 */
void optimize_syntax_tree(std::unique_ptr<Viewspec>& root) {
	namespace V = ViewspecImpl;
	root->run_children(optimize_syntax_tree);
	if (auto* const and_op = dynamic_cast<V::AndOp*>(root.get()); and_op != nullptr) {
		auto* not_op_left = dynamic_cast<V::NotOp*>(and_op->left.get());
		auto* not_op_right = dynamic_cast<V::NotOp*>(and_op->right.get());
		if (not_op_left && not_op_right) {
			// (not a) and (not b) -> not (a or b)
			auto new_or_op = std::make_unique<V::OrOp>(std::move(not_op_left->child), std::move(not_op_right->child));
			root = std::make_unique<V::NotOp>(std::move(new_or_op));
		} else if ((not_op_left != nullptr) ^ (not_op_right != nullptr)) {
			if (not_op_left != nullptr) {
				using std::swap;
				swap(not_op_left, not_op_right);
				swap(and_op->left, and_op->right);
			}
			// a and (not b) -> a subtract b
			root = std::make_unique<V::SubtractOp>(std::move(and_op->left), std::move(not_op_right->child));
		}  // else: a and b -> (leave it alone)
	} else if (auto* const subtract_op = dynamic_cast<V::SubtractOp*>(root.get()); subtract_op != nullptr) {
		auto* not_op_minuend = dynamic_cast<V::NotOp*>(subtract_op->minuend.get());
		auto* not_op_subtrahend = dynamic_cast<V::NotOp*>(subtract_op->subtrahend.get());
		if (not_op_minuend && not_op_subtrahend) {
			// (not a) minus (not b) -> (not a) and b -> b and (not a) -> b minus a
			root = std::make_unique<V::SubtractOp>(std::move(not_op_subtrahend->child), std::move(not_op_minuend->child));
		} else if (not_op_minuend && !not_op_subtrahend) {
			// (not a) minus b -> (not a) and (not b) -> not (a or b)
			auto new_or_op = std::make_unique<V::OrOp>(std::move(not_op_minuend->child), std::move(subtract_op->subtrahend));
			root = std::make_unique<V::NotOp>(std::move(new_or_op));
		} else if (!not_op_minuend && not_op_subtrahend) {
			// a minus (not b) -> a and b
			root = std::make_unique<V::AndOp>(std::move(subtract_op->minuend), std::move(not_op_subtrahend->child));
		}  // else: a minus b -> (leave it alone)
	} else if (auto* const or_op = dynamic_cast<V::OrOp*>(root.get()); or_op != nullptr) {
		auto* not_op_left = dynamic_cast<V::NotOp*>(or_op->left.get());
		auto* not_op_right = dynamic_cast<V::NotOp*>(or_op->right.get());
		if (not_op_left && not_op_right) {
			// (not a) or (not b) -> not (a and b)
			auto new_and_op = std::make_unique<V::AndOp>(std::move(not_op_left->child), std::move(not_op_right->child));
			root = std::make_unique<V::NotOp>(std::move(new_and_op));
		} else if ((not_op_left != nullptr) ^ (not_op_right != nullptr)) {
			if (not_op_left != nullptr) {
				using std::swap;
				swap(not_op_left, not_op_right);
				swap(or_op->left, or_op->right);
			}
			// a or (not b) -> not ((not a) and b) -> not (b and (not a)) -> not (b minus a)
			auto new_subtract_op = std::make_unique<V::SubtractOp>(std::move(not_op_right->child), std::move(or_op->left));
			root = std::make_unique<V::NotOp>(std::move(new_subtract_op));
		}  // else: a or b -> (leave it alone)
	} else if (auto* const not_op = dynamic_cast<V::NotOp*>(root.get()); not_op != nullptr) {
		auto const not_op_child = dynamic_cast<V::NotOp*>(not_op->child.get());
		if (not_op_child) {
			// not (not a) -> a
			root = std::move(not_op_child->child);
		}
	}  // else: (file or tag) -> (leave it alone)
}

std::unique_ptr<Viewspec> make_syntax_tree(std::unique_ptr<tao::pegtl::parse_tree::node>& root, bool const optimize = true) {
	auto syntax_tree = make_syntax_tree_helper(root);
	if (optimize) {
		optimize_syntax_tree(syntax_tree);
	}
	return syntax_tree;
}

}  // namespace SyntaxTree

std::unique_ptr<Viewspec> parse(std::string_view const raw) try {
	P::memory_input<P::tracking_mode::lazy, P::ascii::eol::lf_crlf> input{ raw.begin(), raw.end() };
	assert(P::analyze<Exprs::grammar>() == 0);
	auto parse_tree = P::parse_tree::parse<Exprs::grammar, Exprs::selector>(input);
	assert(parse_tree != nullptr);
	assert(parse_tree->is_root());
	assert(parse_tree->children.size() == 1);
	parse_tree = std::move(parse_tree->children[0]);
	assert(parse_tree != nullptr);
	assert(!parse_tree->is_root());
	return SyntaxTree::make_syntax_tree(parse_tree);
} catch (tao::pegtl::parse_error const& e) {
	throw Errors::Viewspec::ParseFailure{ std::string{ e.message() }, e.positions() };
}

}  // namespace View::Parser
