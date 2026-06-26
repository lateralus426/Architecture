cmake_minimum_required(VERSION 3.10)

project(Architecture)

set(CMAKE_CXX_STANDARD 23)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Renderer backend options
option(USE_OPENGL "Enable OpenGL backend support" ON)
set(OPENGL_VERSION "4" CACHE STRING "OpenGL version to target (2, 3, 4)")
set_property(CACHE OPENGL_VERSION PROPERTY STRINGS 2 3 4)
option(USE_DX9 "Enable DirectX 9 backend support" OFF)
option(USE_DX10 "Enable DirectX 10 backend support" OFF)
option(USE_DX11 "Enable DirectX 11 backend support" OFF)
option(USE_DX12 "Enable DirectX 12 backend support" OFF)
option(USE_VULKAN "Enable Vulkan backend support" OFF)

if(NOT USE_OPENGL AND NOT USE_DX9 AND NOT USE_DX10 AND NOT USE_DX11 AND NOT USE_DX12 AND NOT USE_VULKAN)
    message(FATAL_ERROR "At least one renderer backend must be enabled: USE_OPENGL, USE_DX9, USE_DX10, USE_DX11, USE_DX12, or USE_VULKAN.")
endif()

if(NOT WIN32 AND (USE_DX9 OR USE_DX10 OR USE_DX11 OR USE_DX12))
    message(FATAL_ERROR "DirectX backend support is only available on Windows.")
endif()

if(USE_OPENGL)
    add_compile_definitions(USE_OPENGL OPENGL_VERSION=${OPENGL_VERSION} IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()
if(USE_DX9)
    add_compile_definitions(USE_DX9)
endif()
if(USE_DX10)
    add_compile_definitions(USE_DX10)
endif()
if(USE_DX11)
    add_compile_definitions(USE_DX11)
endif()
if(USE_DX12)
    add_compile_definitions(USE_DX12)
endif()
if(USE_VULKAN)
    add_compile_definitions(USE_VULKAN)
endif()

# Dependencies
include(Dependencies.cmake)

# Projects
add_subdirectory(Core)
add_subdirectory(App)
add_subdirectory(TetrisApp)
add_subdirectory(TetrisGL)