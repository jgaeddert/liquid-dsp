cmake_minimum_required(VERSION 3.27)
project(liquid-dsp VERSION 1.6 LANGUAGES C)

# Package information. This is included in the `config.h` file that we generate
set(PACKAGE_BUGREPORT "joseph@liquidsdr.org" CACHE INTERNAL
  "address where bug reports for this package should be sent."
)
set(PACKAGE_NAME "${PROJECT_NAME}" CACHE INTERNAL "full name of this package")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}" CACHE INTERNAL
  "full name and version of this package"
)
set(PACKAGE_TARNAME "${PROJECT_NAME}" CACHE INTERNAL
  "one symbol short name of this package"
)
set(PACKAGE_URL "" CACHE INTERNAL "home page for this package")
set(PACKAGE_VERSION "${PROJECT_VERSION}" CACHE INTERNAL
  "version of this package"
)

# Ensure the compiler supports the C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Ensure the user isn't using MSVC, because it doesn't fully support the C99 standard
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "The Microsoft Visual Studio C Compiler is not fully compatibile with the C99 standard.
    If you're on windows, please compile with a GNU compiler (MingW64 is recommended)")
endif()

# Add some CLI config options
option(LIQUID_DEBUG_MESSAGES "Enable verbose debug messages (-DDEBUG)" OFF)
option(LIQUID_FFTOVERRIDE "Force internal FFT even if libfftw is available" OFF)
option(LIQUID_SIMDOVERRIDE "Force overriding of SIMD (use portable C code)" OFF)

# Add the modules in the `cmake` directory, and run GenConfig so we can check for required headers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GenConfig)

##
## Liquid sources
##
include(GetSources)
set(liquid_SOURCES
  src/libliquid.c
  ${agc_SOURCES}
  ${audio_SOURCES}
  ${buffer_SOURCES}
  ${channel_SOURCES}
  ${dotprod_SOURCES}
  ${equalization_SOURCES}
  ${fec_SOURCES}
  ${fft_SOURCES}
  ${filter_SOURCES}
  ${framing_SOURCES}
  ${math_SOURCES}
  ${matrix_SOURCES}
  ${modem_SOURCES}
  ${multichannel_SOURCES}
  ${nco_SOURCES}
  ${optim_SOURCES}
  ${quantization_SOURCES}
  ${random_SOURCES}
  ${sequence_SOURCES}
  ${utility_SOURCES}
  ${vector_SOURCES}
)

set(CMAKE_REQUIRED_LINK_OPTIONS -lc;-lm)

#
# Liquid config
#
add_library(liquid_config INTERFACE)
target_include_directories(liquid_config INTERFACE . include)
target_compile_options(liquid_config INTERFACE -Wall -Wno-deprecated-declarations)
if(LIQUID_DEBUG_MESSAGES)
    target_compile_definitions(liquid_config INTERFACE DEBUG)
endif()

#
# Liquid library
#
add_library(liquid OBJECT ${liquid_SOURCES})
target_link_libraries(liquid liquid_config m)

# Link the fec library if available
if(HAVE_LIBFEC)
  target_link_libraries(liquid fec)
endif()

# Link the fftw3f library if available
if(NOT LIQUID_FFTOVERRIDE AND HAVE_FFTW3_H AND HAVE_LIBFFTW3F)
  target_link_libraries(liquid fftw3f)
endif()

# Create shared lib
add_library(liquid-shared SHARED)
target_link_libraries(liquid-shared liquid)

# Create static lib
add_library(liquid-static STATIC)
target_link_libraries(liquid-static liquid)

# Configure shared and static lib properties
set_target_properties(liquid-shared liquid-static PROPERTIES
  OUTPUT_NAME liquid
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
)

# Include the normal GNU installation directories
include(GNUInstallDirs)

#
# TARGET : install - install the libraries and headers into the host system
#
install(TARGETS liquid-shared liquid-static
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid
)

install(FILES include/liquid.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/liquid
)

install(CODE "
message([===[

---------------------------------------------------------
  liquid-dsp was successfully installed.

  On some machines (e.g. Linux) you should rebind your
  libraries by running 'ldconfig' to make the shared
  object available.  You might also need to modify your
  LD_LIBRARY_PATH environment variable to include the
  install directory (e.g. ${CMAKE_INSTALL_PREFIX})

  Please report bugs to ${PACKAGE_BUGREPORT}
---------------------------------------------------------

]===]
)
"
)

#
# TARGET : uninstall - uninstall the libraries and headers from the host system
#
# Generate the cmake_uninstall module
configure_file(cmake/uninstall.CMakeLists.txt.in cmake_uninstall.cmake @ONLY)
# Create the uninstall target and point it to the cmake_uninstall module
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND}
          -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  COMMENT "Uninstall the project..."
)
