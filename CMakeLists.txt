cmake_minimum_required(VERSION 3.8)
project(ForgeGL, VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB source_files CONFIGURE_DEPENDS
"${CMAKE_CURRENT_SOURCE_DIR}/src/engine/renderer/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src/engine/renderer/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/engine/core/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/engine/core/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src/components/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src//*.cpp"
)

# Include directories
include_directories(
    lib/imgui
    lib/imgui/backends
    lib/tracy/public
)

# ImGui source files
set(IMGUI_SRC
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib//imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_subdirectory(lib/tracy)
add_executable(ForgeGL src/main.cpp ${source_files} ${IMGUI_SRC} lib/tracy/public/TracyClient.cpp)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("lib/glfw-3.4")
target_link_libraries(ForgeGL glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(ForgeGL OpenGL::GL)


# GLAD
add_subdirectory("lib/glad/")
target_link_libraries(ForgeGL glad)


# GLM
add_subdirectory("lib/glm")
target_link_libraries(ForgeGL glm)


# STB_IMAGE
add_subdirectory("lib/stb_image/")
target_link_libraries(ForgeGL stb_image)


# ASSIMP
add_subdirectory("lib/assimp/")
target_link_libraries(ForgeGL assimp)

# ReactPhyscis3d
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake/ReactPhysics3D")
find_package(ReactPhysics3D REQUIRED)
target_link_libraries(ForgeGL ReactPhysics3D::ReactPhysics3D)

# Entt
add_subdirectory(lib/entt)
target_link_libraries(ForgeGL EnTT::EnTT)

add_subdirectory(lib/json)
target_link_libraries(ForgeGL nlohmann_json)