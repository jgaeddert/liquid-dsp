find_path(FFTW_INCLUDE_DIR fftw3.h)
find_library(FFTWF_LIBRARY fftw3f)

if(FFTWF_LIBRARY)
  include(CheckLibraryExists)
  set(CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
  set(CMAKE_REQUIRED_QUIET ${FFTW_FIND_QUIETLY})

  get_filename_component(FFTWF_LIBRARY_DIR ${FFTWF_LIBRARY} PATH)
  check_library_exists(fftw3f fftwf_plan_dft_1d
    ${FFTWF_LIBRARY_DIR}
    FFTWF_HAS_FFTWF_PLAN_DFT_1D
  )

  set(CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW
  REQUIRED_VARS FFTWF_LIBRARY FFTW_INCLUDE_DIR FFTWF_HAS_FFTWF_PLAN_DFT_1D
)
mark_as_advanced(FFTWF_LIBRARY FFTW_INCLUDE_DIR)

if(FFTW_FOUND)
  if(NOT TARGET FFTW::fftw3f)
    add_library(FFTW::fftw3f UNKNOWN IMPORTED)
    set_target_properties(FFTW::fftw3f PROPERTIES
      IMPORTED_LOCATION "${FFTWF_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIR}"
    )
  endif()

  set(FFTW_LIBRARIES ${FFTWF_LIBRARY})
  set(FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR})
endif()
