#
# TetrisApp
#

set(SOURCES
    src/main.cpp
    src/TetrisAppLayer.h
    src/TetrisAppLayer.cpp
    src/Models.h
    src/Models.cpp
    src/gl_loader.h
    src/GUILayer.h
    src/GUILayer.cpp
    #src/VoidLayer.h
    #src/VoidLayer.cpp
    #src/OverlayLayer.h
    #src/OverlayLayer.cpp
)

add_executable(TetrisGL)

target_sources(TetrisGL PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

target_link_libraries(TetrisGL Core)
target_link_libraries(TetrisGL imgui)
target_include_directories(TetrisGL PRIVATE src)

# Set working directory for VS debugger
set_target_properties(TetrisGL PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/TetrisGL"
)