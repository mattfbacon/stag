.PHONY: cmake debug release clean distclean install uninstall tidy cpplint cppcheck check

BUILD_DIR := build

CMAKE := cmake

CMAKE_FLAGS := -DCMAKE_EXPORT_COMPILE_COMMANDS=1

export CMAKE_GENERATOR = Ninja Multi-Config

$(BUILD_DIR): CMakeLists.txt
	$(CMAKE) $(CMAKE_FLAGS) -S. -H. -B$(BUILD_DIR)
cmake: $(BUILD_DIR)

debug release: %: $(BUILD_DIR)
# hacky support for multiconfig generators
	$(CMAKE) --build $(BUILD_DIR) --target $@ -- -v\
		|| $(CMAKE) --build $(BUILD_DIR) --config $(shell echo $@ | python -c "print(input().capitalize())") -- -v
clean distclean: %: $(BUILD_DIR)
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

# can be overridden with an environment variable
PREFIX := /usr/local

install:
	install -Dm 755 -s dist/Release/stag $(PREFIX)"/bin/stag"
	install -Dm 644 dist/stag.1 $(PREFIX)"/share/man/man1/stag.1"
	install -Dm 644 dist/stag.5 $(PREFIX)"/share/man/man5/stag.5"
	install -Dm 644 stag.fish $(PREFIX)"/share/fish/vendor_completions.d/stag.fish"
uninstall:
	rm $(PREFIX)"/bin/stag"
	rm $(PREFIX)"/share/man/man1/stag.1"
	rm $(PREFIX)"/share/man/man5/stag.5"
