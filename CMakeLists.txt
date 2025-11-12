cmake_minimum_required(VERSION 3.11-3.18)

project(train_opt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -g -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

include_directories(
	${PROJECT_SOURCE_DIR}/lib 
	${PROJECT_SOURCE_DIR}/src)

file(GLOB SOURCES "src/*.cpp")

add_executable(base_data_test apps/base_data_test.cpp ${SOURCES})
add_executable(solution_test apps/solution_test.cpp ${SOURCES})
