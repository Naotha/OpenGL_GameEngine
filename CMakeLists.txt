cmake_minimum_required(VERSION 3.22.2)
project(OpenGL_GameEngine)

file(GLOB SOURCES
        src/main.cpp
        src/Core/OpenGLContext.cpp
        src/Window/Window.cpp
        src/Gui/EditorGui.cpp
        src/Gui/EditorWidget.cpp
        src/EventHandler/EventHandler.cpp
        vendor/imgui/*.cpp
        vendor/imguizmo/*.cpp
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_opengl3.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

# OpenGL
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME}
        PUBLIC ${OPENGL_INCLUDE_DIRS}
)

# Subdirectories
add_subdirectory(vendor/glm)
add_subdirectory(vendor/GLFW)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/json)
add_subdirectory(vendor/glad)

# Includes
target_include_directories(${PROJECT_NAME}
        PUBLIC vendor/glad
        PUBLIC vendor/KHR
        PUBLIC vendor/glm
        PUBLIC vendor/GLFW/include
        PUBLIC vendor/assimp/include
        PUBLIC vendor/json/include
        PUBLIC vendor/imgui
        PUBLIC vendor/colony
        PUBLIC vendor
        PUBLIC include
        PUBLIC src
)

# Libraries
target_link_directories(${PROJECT_NAME}
        PRIVATE vendor/glfw/src
        PRIVATE vendor/assimp/lib
        PRIVATE vendor/glad
)
target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        glfw
        glad
        assimp
)

# Resources
FILE(COPY resources DESTINATION "${CMAKE_BINARY_DIR}/")