# CMakeLists.txt
cmake_minimum_required(VERSION 3.5)

project(Architecture)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Backend selection
option(USE_OPENGL "Enable OpenGL backend support" ON)
#set(OPENGL_VERSION "3" CACHE STRING "OpenGL version to target (2, 3, 4)")
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
    set(OPENGL_TARGET_VERSION "460" CACHE STRING "OpenGL version (150 200 300 330, 400, 430, 450, 460)")
    set_property(CACHE OPENGL_TARGET_VERSION PROPERTY STRINGS 150 200 300 330 400 430 450 460)

    math(EXPR OPENGL_VERSION_MAJOR "${OPENGL_TARGET_VERSION} / 100")
    math(EXPR OPENGL_VERSION_MINOR "(${OPENGL_TARGET_VERSION} % 100) / 10")
    #add_compile_definitions(USE_OPENGL OPENGL_VERSION=${OPENGL_TARGET_VERSION} IMGUI_IMPL_OPENGL_LOADER_GLAD)

    add_compile_definitions(
        USE_OPENGL
        OPENGL_VERSION=${OPENGL_TARGET_VERSION}
        OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR}
        OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR}
        IMGUI_IMPL_OPENGL_LOADER_GLAD
    )

    message(STATUS "OpenGL target version: ${OPENGL_TARGET_VERSION}")
    message(STATUS "[Test] Major: ${OPENGL_VERSION_MAJOR}, Minor: ${OPENGL_VERSION_MINOR}")
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


# Dependencies.cmake
include(FetchContent)

# GLFW
find_package(glfw3 3.4 QUIET)
if (NOT glfw3_FOUND)
    FetchContent_Declare(
        glfw3
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
    )
    FetchContent_GetProperties(glfw3)
    if (NOT glfw3_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(glfw3)
        add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR})
    endif()
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD
FetchContent_Declare(
    glad
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/Dav1dde/glad/archive/refs/tags/v2.0.8.zip
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(glad)
    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.6)
endif()
set_target_properties(glad PROPERTIES FOLDER "Dependencies")

# GLM
find_package(glm 1.0.1 QUIET)
if (NOT glm_FOUND)
    FetchContent_Declare(
        glm
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
    )
    FetchContent_GetProperties(glm)
    if (NOT glm_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(glm)
        add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
    endif()
endif()
if(TARGET glm)
    set_target_properties(glm PROPERTIES FOLDER "Dependencies")
endif()

# ImGui
FetchContent_Declare(
    imgui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.8.zip
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(imgui)
endif()

#add_(imgui STATIC
#    ${imgui_SOURCE_DIR}/imgui.cpp
#    ${imgui_SOURCE_DIR}/imgui_draw.cpp
#    ${imgui_SOURCE_DIR}/imgui_tables.cpp
#    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
#)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(USE_OPENGL)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )

    if(OPENGL_VERSION_MAJOR EQUAL 2)
        target_sources(imgui PRIVATE
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
        )
    else()
        target_sources(imgui PRIVATE
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
    endif()
    target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
endif()

if(USE_DX9)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
    )
endif()

if(USE_DX10)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx10.cpp
    )
endif()

if(USE_DX11)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    )
endif()

if(USE_DX12)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
    )
endif()

if(USE_VULKAN)
    target_sources(imgui PRIVATE
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )
endif()

set_target_properties(imgui PROPERTIES FOLDER "Dependencies")

