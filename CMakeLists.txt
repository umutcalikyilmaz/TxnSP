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

option(ENABLE_MIP "Enable building the MIPSolver module (requires the third-party library SCIP)" OFF)

if(ENABLE_MIP)

  find_path(SCIP_INCLUDE_DIR NAMES scip/scip.h)
  find_library(SCIP_LIBRARY NAMES scip)

  if(NOT SCIP_INCLUDE_DIR OR NOT SCIP_LIBRARY)
    message(FATAL_ERROR
      "ENABLE_MIP is ON, but SCIP was not found. Please install SCIP to build the project enabling MIP.")
  endif()

endif()

set(SOURCE_FILES src/PoolingModels/SchedulePool.cpp src/PoolingModels/SubsetPool.cpp
src/ProblemModels/Problem.cpp src/ProblemModels/Schedule.cpp src/ProblemModels/Subset.cpp src/Shared/SharedModels.cpp
src/Solvers/DPSolver.cpp src/Solvers/ESSolver.cpp src/Solvers/SASolver.cpp src/Structs/SolverOutput.cpp src/Testers/Analyzer.cpp
src/Testers/Evaluator.cpp)

if(ENABLE_MIP)
  list(APPEND SOURCE_FILES src/Solvers/MIPSolver.cpp)
endif()

add_library(${BINARY_NAME} SHARED ${SOURCE_FILES})
set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${BINARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${BINARY_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1.0.0
  PUBLIC_HEADER include/TxnSP/TxnSP.h    
)

if(ENABLE_MIP)

  target_compile_definitions(${BINARY_NAME} PUBLIC ENABLE_MIP=1)
  target_include_directories(${BINARY_NAME} PRIVATE ${SCIP_INCLUDE_DIR})
  target_link_libraries(${BINARY_NAME} ${SCIP_LIBRARY})

endif()

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/TxnSPConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/TxnSPConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TxnSP
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/TxnSPConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TxnSP)

install(TARGETS ${BINARY_NAME}
  EXPORT TxnSPTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TxnSP
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT TxnSPTargets
  FILE TxnSPTargets.cmake
  NAMESPACE TxnSP::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TxnSP)
