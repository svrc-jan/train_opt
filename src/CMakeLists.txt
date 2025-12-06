find_library(CLP_LIB Clp)
find_library(CLP_SOLVER_LIB ClpSolver)
find_library(COIN_UTILS_LIB CoinUtils)

set(COIN_LIBS
	${CLP_LIB} " "
	${CLP_SOLVER_LIB} " "
	${COIN_UTILS_LIB})

message(STATUS "coin libs: " ${COIN_LIBS})

add_subdirectory(utils)

add_library(instance)
target_sources(instance
	PRIVATE
		instance.cpp
		utils/array.hpp
	PUBLIC
		instance.hpp
)
target_link_libraries(files)


add_library(preprocess)
target_sources(preprocess
	PRIVATE
		preprocess.cpp
	PUBLIC
		preprocess.hpp
)

add_library(graph)
target_sources(graph
	PRIVATE
		graph.cpp
	PUBLIC
		graph.hpp
)
