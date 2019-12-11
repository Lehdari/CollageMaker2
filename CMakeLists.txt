cmake_minimum_required(VERSION 3.0)
project(CollageMaker2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add external dependencies
add_subdirectory(ext)

# Add executable: CollageMaker2
add_subdirectory(include)
add_subdirectory(src)

add_executable(CollageMaker2 ${EXECUTABLE_HEADERS} ${EXECUTABLE_SOURCES})

target_include_directories(CollageMaker2
    PUBLIC
        include)

target_link_libraries(CollageMaker2
    PUBLIC
        OpenGLUtils
)
