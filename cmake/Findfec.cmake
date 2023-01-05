find_path(fec_INCLUDE_DIR fec.h)
find_library(fec_LIBRARY fec)

if(fec_LIBRARY)
  include(CheckLibraryExists)
  set(CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
  set(CMAKE_REQUIRED_QUIET ${fec_FIND_QUIETLY})

  get_filename_component(fec_LIBRARY_DIR ${fec_LIBRARY} PATH)
  check_library_exists(fec create_viterbi27
    ${fec_LIBRARY_DIR}
    fec_HAS_CREATE_VITERBI27
  )

  set(CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(fec
  REQUIRED_VARS fec_LIBRARY fec_INCLUDE_DIR fec_HAS_CREATE_VITERBI27
)
mark_as_advanced(fec_LIBRARY fec_INCLUDE_DIR)

if(fec_FOUND AND NOT TARGET fec::fec)
  add_library(fec::fec UNKNOWN IMPORTED)
  set_target_properties(fec::fec PROPERTIES
    IMPORTED_LOCATION "${fec_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${fec_INCLUDE_DIR}"
  )
endif()
