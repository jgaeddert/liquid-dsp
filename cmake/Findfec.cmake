# find library and header instance of libfec

include(FindPackageHandleStandardArgs)

find_path(fec_INCLUDE_DIR NAMES fec.h
    HINTS
    /usr/include
    /usr/local/include
    ${CMAKE_INSTALL_PREFIX}/include
)

find_library(fec_LIBRARY NAMES fec libfec
    HINTS
    /usr/lib
    /usr/lib/x86_64-linux-gnu
    /usr/local/lib
    ${CMAKE_INSTALL_PREFIX}/lib
)

find_package_handle_standard_args(fec REQUIRED_VARS fec_LIBRARY fec_INCLUDE_DIR)

if (fec_FOUND)
    mark_as_advanced(fec_INCLUDE_DIR)
    mark_as_advanced(fec_LIBRARY)
endif()

if (fec_FOUND AND NOT TARGET fec::fec)
    add_library(fec::fec IMPORTED SHARED)
    set_property(TARGET fec::fec PROPERTY IMPORTED_LOCATION ${fec_LIBRARY})
    target_include_directories(fec::fec INTERFACE ${fec_LIBRARY})
endif()

