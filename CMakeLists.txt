cmake_minimum_required(VERSION 3.15)
project(MKTSIM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/SFML")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui-SFML.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${SFML_ROOT}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${IMGUI_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${SFML_ROOT}/lib/libsfml-graphics.dylib"
    "${SFML_ROOT}/lib/libsfml-window.dylib"
    "${SFML_ROOT}/lib/libsfml-system.dylib"
    "-framework OpenGL"
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path/../external/SFML/lib"
)