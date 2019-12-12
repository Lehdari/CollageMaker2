cmake_minimum_required(VERSION 3.0)
project(CollageMaker2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add external dependencies
add_subdirectory(ext)


# Add project files
add_subdirectory(include)
add_subdirectory(src)


# Define location of the resource folder
# Note: for release build, res folder needs to be copied to build directory
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/")
else ()
    set(RES_DIR "res/")
endif ()


# Add executable: CollageMaker2
add_executable(CollageMaker2 ${EXECUTABLE_HEADERS} ${EXECUTABLE_SOURCES})

target_include_directories(CollageMaker2
    PUBLIC
        include
)

target_link_libraries(CollageMaker2
    PUBLIC
        OpenGLUtils
)

target_compile_definitions(CollageMaker2
    PUBLIC
        RES_DIR=\"${RES_DIR}\"
)

# Enable console printing when making debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(CollageMaker2
        PUBLIC
            -mconsole)
endif()
