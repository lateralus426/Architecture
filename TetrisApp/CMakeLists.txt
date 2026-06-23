#
# TetrisApp
#

set(SOURCES
    src/main.cpp
    src/TetrisAppLayer.h
    src/TetrisAppLayer.cpp
    #src/VoidLayer.h
    #src/VoidLayer.cpp
    #src/OverlayLayer.h
    #src/OverlayLayer.cpp
)

add_executable(TetrisApp)

target_sources(TetrisApp PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

target_link_libraries(TetrisApp Core)

target_include_directories(TetrisApp PRIVATE src)

# Set working directory for VS debugger
set_target_properties(TetrisApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/TetrisApp"
)