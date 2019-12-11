cmake_minimum_required(VERSION 3.0)
project(CollageMaker2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable: CollageMaker2
add_subdirectory(include)
add_subdirectory(src)

add_executable(CollageMaker2 ${EXECUTABLE_HEADERS} ${EXECUTABLE_SOURCES})

target_include_directories(CollageMaker2 PUBLIC include)

# Find and link SFML
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
target_link_libraries(CollageMaker2 sfml-system sfml-window sfml-graphics)