cmake_minimum_required(VERSION 3.28)
Project(Renderer)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB IMGUI ${PROJECT_SOURCE_DIR}/imgui/* ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl2.cpp ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp)

add_library(imgui ${IMGUI})
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/imgui/)
target_include_directories(imgui PUBLIC /usr/include/SDL2/)
target_link_libraries(imgui SDL2 GL)

add_executable(renderer ${SOURCE_FILES})
target_include_directories(renderer PUBLIC ${PROJECT_SOURCE_DIR}/imgui/ ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(renderer SDL2 GLEW GL imgui)
