cmake_minimum_required(VERSION 3.11-3.18)

project(train_opt)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-O2 -Wall -g)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

find_library(CLP_LIB Clp)
find_library(CLP_SOLVER_LIB ClpSolver)
find_library(COIN_UTILS_LIB CoinUtils)


message(STATUS "COIN libs:"
	${CLP_LIB} " "
	${CLP_SOLVER_LIB} " "
	${COIN_UTILS_LIB})

link_libraries(${CLP_LIB} ${CLP_SOLVER_LIB} ${COIN_UTILS_LIB})

include_directories(
	/usr/include
	${PROJECT_SOURCE_DIR}/lib 
	${PROJECT_SOURCE_DIR}/src)

file(GLOB SOURCES "src/*.cpp" "src/utils/*.cpp")

add_executable(instance_test apps/instance_test.cpp ${SOURCES})
add_executable(graph_test apps/graph_test.cpp ${SOURCES})
add_executable(tree_test apps/tree_test.cpp ${SOURCES})
add_executable(path_selector_test apps/path_selector_test.cpp ${SOURCES})


