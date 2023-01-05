cmake_minimum_required(VERSION 3.3)

macro(check_cpuid extension variable)
  if(NOT DEFINED "${variable}")
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Checking for CPU instruction set ${extension}")
    endif()

    if(${extension} IN_LIST _CPUID_AVAILABLE_EXTENSIONS)
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS "Checking for CPU instruction set ${extension} - found")
      endif()
      set(${variable} 1 CACHE INTERNAL "CPU instruction set ${extension}")
    else()
      if(NOT CMAKE_REQUIRED_QUIET)
        message(STATUS
          "Checking for CPU instruction set ${extension} - not found"
        )
      endif()
      set(${variable} "" CACHE INTERNAL "CPU instruction set ${extension}")
    endif()
  endif()
endmacro()

# return early if extensions list already set
if(_CPUID_AVAILABLE_EXTENSIONS)
  return()
endif()

# attempt to check for available extensions
message(STATUS "Checking for available CPU instruction set extensions")
set(_cacheString "Available CPU instruction set extensions")
set(_CPUID_AVAILABLE_EXTENSIONS "" CACHE INTERNAL "${_cacheString}")

if(NOT TARGET CpuFeatures::list_cpu_features)
  find_package(CpuFeatures NO_MODULE REQUIRED)
endif()

get_target_property(CpuFeatures_EXECUTABLE
  CpuFeatures::list_cpu_features
  LOCATION
)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)

  macro(__json_array_to_list outvar jsonArray)
    string(JSON __arrayLen LENGTH "${jsonArray}")
    math(EXPR __arrayLenM1 "${__arrayLen} - 1")
    foreach(__arrayIndex RANGE ${__arrayLenM1})
        string(JSON __currArrayIndex GET "${jsonArray}" ${__arrayIndex})
        list(APPEND ${outvar} ${__currArrayIndex})
   endforeach()
  endmacro()

  execute_process(
    COMMAND "${CpuFeatures_EXECUTABLE}" --json
    RESULT_VARIABLE CpuFeatures_ExitCode
    OUTPUT_VARIABLE CpuFeatures_Json
  )

  if(NOT CpuFeatures_ExitCode)
    string(JSON CpuFeatures_Flags GET "${CpuFeatures_Json}" flags)
    __json_array_to_list(CpuFeatures_FlagList "${CpuFeatures_Flags}")
    set(_CPUID_AVAILABLE_EXTENSIONS ${CpuFeatures_FlagList} CACHE INTERNAL
      "${_cacheString}"
    )
  endif()

else()

  execute_process(
    COMMAND "${CpuFeatures_EXECUTABLE}"
    RESULT_VARIABLE CpuFeatures_ExitCode
    OUTPUT_VARIABLE CpuFeatures_Out
  )

  if(NOT CpuFeatures_ExitCode)
    string(FIND "${CpuFeatures_Out}" "flags" flags_pos)
    string(FIND "${CpuFeatures_Out}" "cache_info" cache_info_pos)
    set(length -1)
    if(NOT cache_info_pos EQUAL -1)
      math(EXPR length "${cache_info_pos} - ${flags_pos}")
    endif()
    string(SUBSTRING "${CpuFeatures_Out}" ${flags_pos} ${length} flags_kv)
    string(FIND "${flags_kv}" ":" kv_sep_pos)
    string(LENGTH ":" kv_sep_len)
    math(EXPR kv_post_sep_pos "${kv_sep_pos} + ${kv_sep_len}")
    string(SUBSTRING "${flags_kv}" ${kv_post_sep_pos} -1 flags_content)
    string(STRIP "${flags_content}" flags_content)
    string(REPLACE "," ";" CpuFeatures_FlagList "${flags_content}")
    set(_CPUID_AVAILABLE_EXTENSIONS ${CpuFeatures_FlagList} CACHE INTERNAL
      "${_cacheString}"
    )
  endif()

endif()

if(NOT _CPUID_AVAILABLE_EXTENSIONS)
  message(AUTHOR_WARNING "_CPUID_AVAILABLE_EXTENSIONS set to empty list")
endif()
