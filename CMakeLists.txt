cmake_minimum_required(VERSION 3.11-3.18)

project(train_opt)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_PREFIX_PATH "/usr/local/lib")
set(CMAKE_CXX_FLAGS "-Wall -g -O3 ${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

find_library(SOPLEX_LIB soplex)
cmake_print_variables(SOPLEX_LIB)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

include_directories(
	/usr/local/include
	${PROJECT_SOURCE_DIR}/lib 
	${PROJECT_SOURCE_DIR}/src)

file(GLOB SOURCES "src/*.cpp" "src/utils/*.cpp")

add_executable(instance_test apps/instance_test.cpp ${SOURCES})
add_executable(graph_test apps/graph_test.cpp ${SOURCES})
add_executable(tree_test apps/tree_test.cpp ${SOURCES})
add_executable(soplex_test apps/soplex_test.cpp ${SOURCES})

target_link_libraries(soplex_test -lz)
target_link_libraries(soplex_test "/usr/local/lib/libsoplex.a")