find_package(OpenMP)
if (OpenMP_CXX_FOUND)
	add_library(openmp ALIAS OpenMP::OpenMP_CXX)
else()
	add_library(openmp INTERFACE) # empty interface
	message(WARNING "OpenMP was not found; the resulting build will be slower.")
endif()
