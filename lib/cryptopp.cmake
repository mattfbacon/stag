set(BUILD_STATIC ON)
set(BUILD_TESTING OFF)
set(BUILD_SHARED OFF)
set(BUILD_DOCUMENTATION OFF)
set(build_files
	CMakeLists.txt
	cryptopp-config.cmake
	)
list(TRANSFORM build_files PREPEND ${CMAKE_CURRENT_LIST_DIR}/cryptopp/cryptopp-cmake/)
file(COPY ${build_files} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/cryptopp/cryptopp/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/cryptopp/cryptopp)
add_library(cryptopp EXCLUDE_FROM_ALL INTERFACE)
target_include_directories(cryptopp SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/cryptopp/)
target_link_libraries(cryptopp INTERFACE cryptopp-static)
