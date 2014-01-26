find_package (CARES REQUIRED)
find_package (Boost REQUIRED COMPONENTS
	system)
include_directories (
	${CARES_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS})
add_executable (resolve resolve.cpp)
target_link_libraries (resolve
	${CARES_LIBRARY}
	${Boost_LIBRARIES})