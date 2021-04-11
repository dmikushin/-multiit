cmake_minimum_required(VERSION 3.12)

project(multiit)

if (UNIX)
# Use $ORIGIN or @loader_path, which allow to pick up
# dependencies relative to the executable folder
if (APPLE)
set(CMAKE_INSTALL_RPATH "@loader_path")
else()
set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
endif()

set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}_test "src/test/${PROJECT_NAME}_test.cpp")
set_property(TARGET ${PROJECT_NAME}_test PROPERTY CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})

