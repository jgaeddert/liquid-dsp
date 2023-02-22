cmake_minimum_required(VERSION 3.14)

get_cmake_property(_cmake_role CMAKE_ROLE)
if(NOT _cmake_role STREQUAL "PROJECT")
  message(FATAL_ERROR "Script must be ran from an including project.")
endif()

include_guard(GLOBAL)

set(_INSTALL_CPU_FEATURES_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(cpu_features_PREFIX "${CMAKE_BINARY_DIR}/cpu_features-prefix")
set(cpu_features_SUBBUILD "${cpu_features_PREFIX}/src/cpu_features-subbuild")

configure_file(
  "${_INSTALL_CPU_FEATURES_BASE_DIR}/cpu_features.CMakeLists.txt.in"
  "${cpu_features_SUBBUILD}/CMakeLists.txt"
)

execute_process(
  COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY "${cpu_features_SUBBUILD}"
)
if(result)
  message(FATAL_ERROR "CMake step for cpu_features failed: ${result}")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY "${cpu_features_SUBBUILD}"
)
if(result)
  message(FATAL_ERROR "build step for cpu_features failed: ${result}")
endif()

find_package(CpuFeatures NO_MODULE REQUIRED HINTS "${cpu_features_PREFIX}")
