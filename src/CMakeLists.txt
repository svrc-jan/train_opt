find_library(CLP_LIB Clp)
find_library(CLP_SOLVER_LIB ClpSolver)
find_library(COIN_UTILS_LIB CoinUtils)

set(COIN_LIBS
	${CLP_LIB} " "
	${CLP_SOLVER_LIB} " "
	${COIN_UTILS_LIB})

message(STATUS "coin libs: " ${COIN_LIBS})

add_subdirectory(utils)

add_library(instance instance.cpp)
target_link_libraries(instance files)


add_library(preprocess preprocess.cpp)
target_link_libraries(preprocess instance)


add_library(graph graph.cpp)
target_link_libraries(graph preprocess instance)
