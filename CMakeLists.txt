cmake_minimum_required(VERSION 3.5.0)

SET(PROJECT_NAME "TxnSP")
SET(BINARY_NAME "txnsp")

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

add_library(${BINARY_NAME} SHARED src/PoolingModels/SchedulePool.cpp src/PoolingModels/SubsetPool.cpp
src/ProblemModels/Problem.cpp src/ProblemModels/Schedule.cpp src/ProblemModels/Subset.cpp src/Shared/Decode.cpp
src/Shared/List.cpp src/Shared/Math.cpp src/Shared/Random.cpp src/Solvers/DPSolver.cpp src/Solvers/ESSolver.cpp
src/Solvers/MIPSolver.cpp src/Solvers/SASolver.cpp src/Structs/SolverOutput.cpp src/Testers/Analyzer.cpp 
src/Testers/Evaluator.cpp)

set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${BINARY_NAME} PUBLIC include)
target_include_directories(${BINARY_NAME} PRIVATE src)

set_target_properties(${BINARY_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1.0.0
  PUBLIC_HEADER include/TxnSPLib.h    
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/scip/include)
target_link_libraries(${BINARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/scip/lib/libscip.so)

include(GNUInstallDirs)
ADD_DEFINITIONS("-g")

set(TEST
  include/TxnSPTest.h
)

set(POOLING
  include/PoolingModels/SchedulePool.h
  include/PoolingModels/SubsetPool.h
)

set(PROBLEM
  include/ProblemModels/Problem.h
  include/ProblemModels/Schedule.h
  include/ProblemModels/Subset.h
)

set(SHARED
  include/Shared/Decode.h
  include/Shared/List.h
  include/Shared/Math.h
  include/Shared/Parameters.h
  include/Shared/Random.h
)

set(SOLVERS
  include/Solvers/DPSolver.h
  include/Solvers/ESSolver.h
  include/Solvers/MIPSolver.h
  include/Solvers/SASolver.h
  include/Solvers/Solver.h
)

set(STRUCTS
  include/Structs/AnalyzerInput.h
  include/Structs/EvaluatorInput.h
  include/Structs/SolverInput.h
  include/Structs/SolverOutput.h  
)

set(TESTERS
  include/Testers/Analyzer.h
)

install(TARGETS ${BINARY_NAME}
  LIBRARY DESTINATION $ENV{HOME}/TxnSP/lib
  PUBLIC_HEADER DESTINATION $ENV{HOME}/TxnSP/include
)

install(FILES ${TEST} DESTINATION $ENV{HOME}/TxnSP/include COMPONENT Headers)
install(FILES ${POOLING} DESTINATION $ENV{HOME}/TxnSP/include/PoolingModels COMPONENT Headers)
install(FILES ${PROBLEM} DESTINATION $ENV{HOME}/TxnSP/include/ProblemModels COMPONENT Headers)
install(FILES ${SHARED} DESTINATION $ENV{HOME}/TxnSP/include/Shared COMPONENT Headers)
install(FILES ${SOLVERS} DESTINATION $ENV{HOME}/TxnSP/include/Solvers COMPONENT Headers)
install(FILES ${STRUCTS} DESTINATION $ENV{HOME}/TxnSP/include/Structs COMPONENT Headers)
install(FILES ${TESTERS} DESTINATION $ENV{HOME}/TxnSP/include/Testers COMPONENT Headers)
