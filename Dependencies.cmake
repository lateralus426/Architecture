#
# Dependencies
#
include(FetchContent)

# GLFW
find_package(glfw3 3.4 QUIET)
set(ARCHTETRIS_GLFW_INCLUDE_DIR "")
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
    if(EXISTS "${glfw3_SOURCE_DIR}/include")
        set(ARCHTETRIS_GLFW_INCLUDE_DIR "${glfw3_SOURCE_DIR}/include")
    endif()
else()
    if(DEFINED GLFW_INCLUDE_DIRS)
        set(ARCHTETRIS_GLFW_INCLUDE_DIR "${GLFW_INCLUDE_DIRS}")
    elseif(DEFINED GLFW3_INCLUDE_DIRS)
        set(ARCHTETRIS_GLFW_INCLUDE_DIR "${GLFW3_INCLUDE_DIRS}")
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
    FetchContent_MakeAvailable(glad)

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


# imgui
FetchContent_Declare(
    imgui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.8.zip
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

if(USE_OPENGL OR USE_VULKAN)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )
endif()

if(USE_OPENGL)
    if(OPENGL_VERSION STREQUAL "2")
        list(APPEND IMGUI_SOURCES
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
        )
    else()
        list(APPEND IMGUI_SOURCES
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        )
    endif()
endif()

if(USE_VULKAN)
    find_package(Vulkan REQUIRED)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )
endif()

if(USE_DX9 OR USE_DX10 OR USE_DX11 OR USE_DX12)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
endif()
if(USE_DX9)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
    )
endif()
if(USE_DX10)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx10.cpp
    )
endif()
if(USE_DX11)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
    )
endif()
if(USE_DX12)
    list(APPEND IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
    )
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(ARCHTETRIS_GLFW_INCLUDE_DIR)
    target_include_directories(imgui PRIVATE "${ARCHTETRIS_GLFW_INCLUDE_DIR}")
endif()

if(TARGET glfw)
    target_link_libraries(imgui PUBLIC glfw)
endif()

if(USE_VULKAN)
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan)
endif()
if(USE_DX9)
    target_link_libraries(imgui PUBLIC d3d9 dxguid)
endif()
if(USE_DX10)
    target_link_libraries(imgui PUBLIC d3d10 d3d10_1 dxgi)
endif()
if(USE_DX11)
    target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler)
endif()
if(USE_DX12)
    target_link_libraries(imgui PUBLIC d3d12 dxgi d3dcompiler)
endif()

set_target_properties(glm PROPERTIES FOLDER "Dependencies")
set_target_properties(imgui PROPERTIES FOLDER "Dependencies")