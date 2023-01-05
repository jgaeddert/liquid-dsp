cmake_minimum_required(VERSION 3.10)

include_guard(GLOBAL)

set(_CHECK_INLINE_SUPPORT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

macro(check_inline_support variable)
  if(NOT DEFINED "${variable}")
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Checking for inline support")
    endif()

    foreach(keyword "inline" "__inline__" "__inline")
      try_compile(${variable}
        "${CMAKE_BINARY_DIR}"
        "${_CHECK_INLINE_SUPPORT_BASE_DIR}/CheckInlineSupport.c"
        COMPILE_DEFINITIONS "-Dinline=${keyword}"
        OUTPUT_VARIABLE OUTPUT
      )
      if(${variable})
        break()
      endif()
    endforeach()

    if(${variable})
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS "Checking for inline support - found")
      endif()
      set(${variable} 1 CACHE INTERNAL "Have inline support")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Checking for inline support passed with the following output:\n"
        "${OUTPUT}\n\n"
      )
    else()
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS "Checking for inline support - not found")
      endif()
      set(${variable} "" CACHE INTERNAL "Have inline support")
      file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Checking for inline support failed with the following output:\n"
        "${OUTPUT}\n\n"
      )
    endif()
  endif()
endmacro()
