cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCES "*.c" "*/src/*.c")

if(IDF_VERSION_MAJOR GREATER_EQUAL 4)

    idf_component_register( SRCS ${SOURCES}
                            INCLUDE_DIRS "include")

else()

    project(liquid-dsp LANGUAGES C VERSION 0.1)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
    target_include_directories(${PROJECT_NAME} PUBLIC include)
    target_compile_features(${PROJECT_NAME} PRIVATE c_std_99) # we use inline comments and a couple of other c99 things, so we need c99+

endif()