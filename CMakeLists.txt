cmake_minimum_required(VERSION 3.21)

# Names
set(PROJECT_NAME "Stag" CACHE STRING "Project Name")
set(EXECUTABLE_NAME stag CACHE STRING "Executable Name")

# Dirs
set(DIR_SRC src CACHE PATH "Source Directory")
set(DIR_DIST dist CACHE PATH "Binary Directory")
set(DIR_INCLUDE include CACHE PATH "Include Directory")
set(DIR_SYSINC sysinc CACHE PATH "System Include Directory")
set(DIR_LIB lib CACHE PATH "Libraries Directory")

# Sources
set (sources
	ArgParser.cpp
	Callbacks.cpp
	Commands/Fix.cpp
	Commands/Import.cpp
	Commands/MetaTag.cpp
	Commands/Open.cpp
	Commands/Query.cpp
	Commands/TagFiles.cpp
	Commands/Views.cpp
	Errors/Abbreviation.cpp
	Errors/ArgParse.cpp
	Errors/Basic.cpp
	Errors/Commands.cpp
	Errors/OS.cpp
	Errors/Tags.cpp
	Errors/Viewspec.cpp
	Filesystem.cpp
	Filesystem/FilesSet.cpp
	info.cpp
	Logging.cpp
	Logic/AllDirectory.cpp
	Logic/TagDirectory.cpp
	Logic/TagIterator.cpp
	Logic/TagsIterator.cpp
	Logic/TagsSet.cpp
	Logic/UUID.cpp
	main.cpp
	View/Parser.cpp
	View/View.cpp
	View/ViewspecImpl.cpp
	your_stdlib_is_garbage.cpp
	)

project(${PROJECT_NAME} VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_POSITION_INDEPENDENT_CODE True)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

list(TRANSFORM sources PREPEND ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_SRC}/)

function(make_man_page relative_output relative_input)
	set(full_dir ${${PROJECT_NAME}_SOURCE_DIR}/)
	set(input ${full_dir}/${relative_input})
	set(output ${full_dir}/${relative_output})
	add_custom_command(OUTPUT ${output} COMMAND ronn -rw < ${input} > ${output} MAIN_DEPENDENCY ${input})
endfunction()

set(man_pages stag.1 stag.5)
foreach(man_page IN LISTS man_pages)
	make_man_page(${DIR_DIST}/${man_page} ${man_page}.ronn)
endforeach()

list(TRANSFORM man_pages PREPEND ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_DIST}/)
add_executable(${EXECUTABLE_NAME} ${sources} ${man_pages})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_INCLUDE})
target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_SYSINC})

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/${DIR_DIST}")

target_compile_options(${EXECUTABLE_NAME} PRIVATE
-Wall -Wextra
-Wno-unknown-pragmas
-Wcpp -Wshadow -Wundef
-fno-common -Wconversion
-Wno-enum-compare
-frtti -fexceptions

"$<$<CXX_COMPILER_ID:GNU>:\
-fdiagnostics-color\
>"

"$<$<CXX_COMPILER_ID:Clang>:\
-fcolor-diagnostics;\
-Wno-gnu-zero-variadic-macro-arguments;\
-Wno-c++11-extensions;-Wno-c++14-extensions;-Wno-c++17-extensions;-Wno-c++20-extensions;\
-Wno-unused-command-line-argument\
>"

"$<$<CONFIG:Debug>:\
-fno-omit-frame-pointer;\
-fstack-protector-all;\
-fstack-clash-protection;\
-O0;\
-ggdb3;\
-Werror;\
-Wfatal-errors;\
-Wno-error=cpp;\
-Wno-error=pedantic;\
-fasynchronous-unwind-tables;\
-finstrument-functions;\
-fsanitize=address;\
-fsanitize=undefined;\
-fsanitize=leak\
>")

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
"MSGPACK_DISABLE_LEGACY_CONVERT"

"$<$<CONFIG:Release>:\
NDEBUG\
>"

"$<$<CONFIG:Debug>:\
_GLIBCXX_DEBUG;\
_GLIBCXX_ASSERTIONS\
>")

target_link_options(${EXECUTABLE_NAME} PRIVATE
"-Wl,-pie,-z,defs,-z,now,-z,relro"
"-fuse-ld=lld"

"$<$<CONFIG:Debug>:\
-fsanitize=address;\
-fsanitize=undefined;\
-fsanitize=leak\
>")

# Extra Stuff Starts Here
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_SRC}/)

function(addlib)
	foreach(name ${ARGV})
		include(${DIR_LIB}/${name}.cmake)
		target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${name})
	endforeach()
endfunction()

addlib(magic_enum cryptopp openmp pegtl spdlog)
