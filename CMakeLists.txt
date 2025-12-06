cmake_minimum_required(VERSION 3.13)

project(train_opt)

set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -g)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

set(JSON_HEADER ${PROJECT_SOURCE_DIR}/lib/nlohmann/json.hpp)
message(STATUS "json header: " ${JSON_HEADER})

include_directories(
	/usr/include
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/lib)


add_subdirectory(src)

add_executable(instance_test apps/instance_test.cpp)
target_link_libraries(instance_test instance)

# add_executable(graph_test apps/graph_test.cpp ${SOURCES})
# add_executable(tree_test apps/tree_test.cpp ${SOURCES})
# add_executable(path_selector_test apps/path_selector_test.cpp ${SOURCES})
# add_executable(preprocess_test apps/preprocess_test.cpp ${SOURCES})
# add_executable(lin_prog_test apps/lin_prog_test.cpp ${SOURCES})

