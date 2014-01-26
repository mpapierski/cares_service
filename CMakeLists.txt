project (cares_service)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include_directories (
	include/)

add_subdirectory (examples)