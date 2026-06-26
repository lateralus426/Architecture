

# TetrisApp/CMakeLists.txt
set(SOURCES
    src/main.cpp
    src/TetrisAppLayer.h
    src/TetrisAppLayer.cpp
    src/Models.h
    src/Models.cpp
    src/gl_loader.h
    src/GUILayer.h
    src/GUILayer.cpp
)

add_executable(TetrisGL)

target_sources(TetrisGL PRIVATE ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_compile_definitions(TetrisGL PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(TetrisGL PRIVATE Core imgui)
target_include_directories(TetrisGL PRIVATE src)

set_target_properties(TetrisGL PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/TetrisGL"
)