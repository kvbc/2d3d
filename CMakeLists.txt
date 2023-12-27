cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(2d3d)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("${CMAKE_SOURCE_DIR}/external/raylib" raylib EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/imgui" imgui EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/rlImGui" rlImGui EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/libtess2" libtess2 EXCLUDE_FROM_ALL)

file(GLOB APP_SOURCES app/src/*.cpp)
add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(${PROJECT_NAME} raylib imgui rlImGui libtess2)