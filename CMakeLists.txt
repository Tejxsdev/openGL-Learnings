cmake_minimum_required(VERSION 3.8)
project(openGL, VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB source_files CONFIGURE_DEPENDS
"${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Renderer/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Renderer/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/Objects/*.h"
)
add_executable(openGL src/main.cpp ${source_files})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("lib/glfw-3.4")
target_link_libraries(openGL glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(openGL OpenGL::GL)


# GLAD
add_subdirectory("lib/glad/")
target_link_libraries(openGL glad)


# GLM
add_subdirectory("lib/glm")
target_link_libraries(openGL glm)


# STB_IMAGE
add_subdirectory("lib/stb_image/")
target_link_libraries(openGL stb_image)


# ASSIMP
add_subdirectory("lib/assimp/")
target_link_libraries(openGL assimp)

# ReactPhyscis3d
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake/ReactPhysics3D")
find_package(ReactPhysics3D REQUIRED)
target_link_libraries(openGL ReactPhysics3D::ReactPhysics3D)
