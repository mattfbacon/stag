#include <memory>
#include <string_view>

#include "View/Viewspec.hpp"

namespace View::Parser {

[[nodiscard]] std::unique_ptr<Viewspec> parse(std::string_view raw);

}
