add_library(magic_enum EXCLUDE_FROM_ALL INTERFACE)
target_include_directories(magic_enum SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/magic_enum/include)
