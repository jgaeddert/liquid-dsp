function(get_liquid_version HEADER_FILE OUTPUT_VAR)
    if (NOT EXISTS "${HEADER_FILE}")
        message(FATAL_ERROR "No ${HEADER_FILE} found!")
    endif()

    file(READ "${HEADER_FILE}" HEADER_CONTENTS)

    string(REGEX MATCH "#define[ \t]+LIQUID_VERSION_MAJOR[ \t]+([0-9]+)" _ ${HEADER_CONTENTS})
    set(VERSION_MAJOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "#define[ \t]+LIQUID_VERSION_MINOR[ \t]+([0-9]+)" _ ${HEADER_CONTENTS})
    set(VERSION_MINOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "#define[ \t]+LIQUID_VERSION_PATCH[ \t]+([0-9]+)" _ ${HEADER_CONTENTS})
    set(VERSION_PATCH ${CMAKE_MATCH_1})

    if (VERSION_MAJOR STREQUAL "" OR VERSION_MINOR STREQUAL "" OR VERSION_PATCH STREQUAL "")
        message(FATAL_ERROR "Could not extract version from ${HEADER_FILE}!")
    endif()

    set(${OUTPUT_VAR} "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" PARENT_SCOPE)
endfunction()
