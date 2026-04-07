cmake_minimum_required(VERSION 3.15)
project(MKTSIM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/SFML")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${SFML_ROOT}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${SFML_ROOT}/lib/libsfml-graphics.dylib"
    "${SFML_ROOT}/lib/libsfml-window.dylib"
    "${SFML_ROOT}/lib/libsfml-system.dylib"
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path/../external/SFML/lib"
)