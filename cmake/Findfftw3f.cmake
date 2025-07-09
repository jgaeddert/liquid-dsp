# find library and header instance of libfftw3f (fast Fourier transform)

include(FindPackageHandleStandardArgs)

find_path(fftw3f_INCLUDE_DIR NAMES fftw3.h
    HINTS
    /usr/include
    /usr/local/include
    ${CMAKE_INSTALL_PREFIX}/include
)

find_library(fftw3f_LIBRARY NAMES fftw3f libfftw3f
    HINTS
    /usr/lib
    /usr/lib/x86_64-linux-gnu
    /usr/local/lib
    ${CMAKE_INSTALL_PREFIX}/lib
)

find_package_handle_standard_args(fftw3f REQUIRED_VARS fftw3f_LIBRARY fftw3f_INCLUDE_DIR)

if (fftw3f_FOUND)
    mark_as_advanced(fftw3f_INCLUDE_DIR)
    mark_as_advanced(fftw3f_LIBRARY)
endif()

if (fftw3f_FOUND AND NOT TARGET fftw::fftw)
    add_library(fftw::fftw IMPORTED SHARED)
    set_property(TARGET fftw::fftw PROPERTY IMPORTED_LOCATION ${fftw3f_LIBRARY})
    target_include_directories(fftw::fftw INTERFACE ${fftw3f_LIBRARY})
endif()

