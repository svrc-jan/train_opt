cmake_minimum_required(VERSION 3.11-3.18)

project(train_opt)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

include_directories(
	${PROJECT_SOURCE_DIR}/apps
	${PROJECT_SOURCE_DIR}/src 
	${PROJECT_SOURCE_DIR}/lib)

add_library(utils src/utils.cpp)

add_library(instance src/instance.cpp)
target_link_libraries(instance PRIVATE utils)

add_executable(instance_test apps/instance_test.cpp)
target_link_libraries(instance_test PRIVATE instance)
