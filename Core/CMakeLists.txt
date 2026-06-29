#
# Core
#

set(SOURCES
    vendor/stb/stb_image.h
    vendor/stb/stb_image.cpp
    Source/Core/Application.h
    Source/Core/Application.cpp
    Source/Core/Window.h
    Source/Core/Window.cpp
    Source/Core/Layer.h
    Source/Core/Layer.cpp
    Source/Core/Event.h
    Source/Core/WindowEvents.h
    Source/Core/InputEvents.h
    Source/Core/Renderer/Renderer.h
    Source/Core/Renderer/Renderer.cpp
    Source/Core/Renderer/Shader.h
    Source/Core/Renderer/Shader.cpp
    Source/Core/Renderer/GLUtils.h
    Source/Core/Renderer/GLUtils.cpp
)

add_library(Core STATIC)

target_sources(Core PRIVATE ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

# -----------------------------
# Compile definitions (FIXED)
# -----------------------------

target_compile_definitions(Core PUBLIC
    GLFW_INCLUDE_NONE
)

if(USE_OPENGL)

    target_compile_definitions(Core PUBLIC
        USE_OPENGL=1
        OPENGL_VERSION=${OPENGL_TARGET_VERSION}
        OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR}
        OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR}
    )

endif()

# -----------------------------
# Dependencies
# -----------------------------

target_link_libraries(Core PUBLIC glfw glad glm)

if(ARCHTETRIS_GLFW_INCLUDE_DIR)
    target_include_directories(Core PUBLIC "${ARCHTETRIS_GLFW_INCLUDE_DIR}")
endif()

target_include_directories(Core PUBLIC
    "Source"
    "vendor/stb"
)