cmake_minimum_required(VERSION 3.27)
project(Renderer LANGUAGES CXX C)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB WAYLAND_FILES ${PROJECT_SOURCE_DIR}/src/wayland/*)

add_subdirectory(tools)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  file(GLOB IMGUI ${PROJECT_SOURCE_DIR}/imgui/*
       ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.cpp
       ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
  add_library(imgui ${IMGUI})
  target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/imgui/)
  target_link_libraries(imgui SDL3::SDL3 GL)
  set(IMGUI_LIBRARY imgui)
endif()

add_executable(renderer ${SOURCE_FILES} ${WAYLAND_FILES})
target_include_directories(renderer PUBLIC ${PROJECT_SOURCE_DIR}/imgui/
                                           ${PROJECT_SOURCE_DIR}/include)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    target_compile_definitions(renderer PUBLIC _DEBUG)
endif()

target_compile_definitions(renderer PUBLIC WL3D)
add_subdirectory(protocols)

target_link_libraries(renderer SDL3::SDL3 GLEW GL ${IMGUI_LIBRARY} wayland-server xkbcommon)

add_custom_target(run DEPENDS renderer WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${PROJECT_SOURCE_DIR}/build/renderer ${PROJECT_SOURCE_DIR}/ressources/box.mod)
