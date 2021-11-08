.PHONY: cmake debug release clean distclean install tidy cpplint cppcheck check

BUILD_DIR := build

CMAKE := cmake

CMAKE_FLAGS :=

export CMAKE_GENERATOR = Ninja Multi-Config

$(BUILD_DIR): CMakeLists.txt
	$(CMAKE) $(CMAKE_FLAGS) -S. -H. -B$(BUILD_DIR)
cmake: $(BUILD_DIR)

debug release: %: $(BUILD_DIR)
# hacky support for multiconfig generators
	$(CMAKE) --build $(BUILD_DIR) --target $@ -- -v\
		|| $(CMAKE) --build $(BUILD_DIR) --config $(shell echo $@ | python -c "print(input().capitalize())") -- -v
clean distclean install: %: $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR) --target $@

.DEFAULT_GOAL = release

ALLSOURCES := $(shell find src include -name '*.cpp' -or -name '*.hpp')

TIDY_DIR := .clangtidy-check
$(TIDY_DIR)/%: % .clang-tidy compile_commands.json
	mkdir -p $(shell dirname $@)
	clang-tidy $< > $@
	test -s $@ || rm $@

tidy: $(addprefix $(TIDY_DIR)/,$(ALLSOURCES))
	find $(TIDY_DIR) -empty -delete

cpplint: $(ALLSOURCES)
	cpplint --filter=-whitespace,-legal,-build/include_order,-build/c++11,-runtime/references,-readability/nolint --quiet $^

cppcheck: $(ALLSOURCES)
	cppcheck $^ --enable=all -q --suppress=unusedFunction --suppress=missingIncludeSystem -Iinclude -x c++ --inline-suppr

check: cpplint cppcheck
