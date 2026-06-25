#pragma once

//#include "gl_loader.h"
//#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <array>



// ==================== Compile-Time API Selection ====================
#define GL3_PLUS_SUPPORT

// ==================== Constants ====================

static const int BOARD_WIDTH = 10;
static const int BOARD_HEIGHT = 20;
static const float CELL_SIZE = 1.0f;

// ==================== Color Types ====================

enum class ColorId : int 
{
    EMPTY = 0,
    I = 1, O = 2, T = 3, S = 4, Z = 5, J = 6, L = 7
};

// Color lookup table for rendering
const std::array<float, 3> PIECE_COLORS[] = 
{
    {0.0f, 1.0f, 1.0f},  // I - cyan
    {1.0f, 1.0f, 0.0f},  // O - yellow
    {0.8f, 0.0f, 0.8f},  // T - purple
    {0.0f, 1.0f, 0.0f},  // S - green
    {1.0f, 0.0f, 0.0f},  // Z - red
    {0.0f, 0.0f, 1.0f},  // J - blue
    {1.0f, 0.6f, 0.0f}   // L - orange
};

// ==================== Tetromino Definitions ====================

enum class PieceType {
    I, O, T, S, Z, J, L
};

static const std::vector<std::pair<int, int>> BASE_CELLS[] = {
    {{0,0}, {0,1}, {0,2}, {0,3}},  // I
    {{0,0}, {0,1}, {1,0}, {1,1}},  // O
    {{0,1}, {1,0}, {1,1}, {1,2}},  // T
    {{0,1}, {0,2}, {1,0}, {1,1}},  // S
    {{0,0}, {0,1}, {1,1}, {1,2}},  // Z
    {{0,0}, {1,0}, {2,0}, {1,1}},  // J
    {{0,2}, {1,0}, {1,1}, {1,2}}   // L
};

// ==================== Game State (all int grid space) ====================

struct GameState 
{
    // 2D grid: 0 = empty, 1-7 = piece type
    int m_Grid[BOARD_HEIGHT][BOARD_WIDTH];

    // Current piece in int coordinates
    PieceType m_CurrentType;
    std::vector<std::pair<int, int>> m_CurrentCells;
    int m_CurrentRow, m_CurrentCol;

    // Next piece
    PieceType m_NextType;
    std::vector<std::pair<int, int>> m_NextCells;

    int m_Score = 0;
    int m_Level = 1;
    int m_LinesCleared = 0;
    int m_DropInterval = 1000; // ms
    double m_LastDropTime = 0;
    bool m_GameOver = false;
    bool m_Paused = false;
    int m_iRotationToggle = 0;

    std::mt19937 m_Rng;

    GameState() : m_Rng(std::chrono::steady_clock::now().time_since_epoch().count()) 
    {
        ResetGrid();
    }

    void ResetGrid();
    PieceType RandomPieceType();

    void LoadPiece(PieceType type);

    void LoadNextPiece();

    [[nodiscard]] bool IsValidPosition(int row, int col, const std::vector<std::pair<int, int>>& cells) const;
    void PlacePiece();
    void ClearLines();
    void MoveDown();
    void MoveLeft();
    void MoveRight();
    void Rotate();
    void Init();
    void HardDrop();
};

// ==================== Render Data (pure data, no logic) ====================

struct RenderCell {
    int x, y;
    int colorId; // 0 = empty, 1-7 = piece
};

struct RenderPiece {
    PieceType type;
    const std::vector<std::pair<int, int>>& cells;
    int row, col;
};

// ==================== OpenGL Rendering (pure function) ====================

struct Vertex {
    float x, y, z;
    float r, g, b;
};

const int VERTS_PER_QUAD = 6;

// ==================== Main ====================

//int main() {
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW" << std::endl;
//        return -1;
//    }
//
//    int width = 1024;
//    int height = 768;
//
//#ifdef GL3_PLUS_SUPPORT
//    std::cout << "Requesting OpenGL 3.3 Core Profile\n";
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//#else
//    std::cout << "Requesting OpenGL 2.1 Legacy Profile\n";
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//#endif
//
//    GLFWwindow* window = glfwCreateWindow(width, height, "Tetris OpenGL", nullptr, nullptr);
//    if (!window) {
//        std::cerr << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    glfwMakeContextCurrent(window);
//    glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int width, int height) {
//        glViewport(0, 0, width, height);
//        (void)win;
//        });
//
//    glewExperimental = GL_TRUE;
//    if (glewInit() != GLEW_OK) {
//        std::cerr << "Failed to initialize GLEW" << std::endl;
//        return -1;
//    }
//
//    glViewport(0, 0, width, height);
//    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
//
//    Renderer renderer;
//    renderer.init();
//
//#ifdef GL3_PLUS_SUPPORT
//    renderer.setupShader("shaders/modern_vs.glsl", "shaders/modern_fs.glsl");
//#else
//    renderer.setupShader("shaders/legacy_vs.glsl", "shaders/legacy_fs.glsl");
//#endif
//    std::cout << "Finished compiling the shaders\n";
//
//    GameState state;
//    state.Init();
//
//    
//
//    // One-shot keys: trigger once per press (UP, SPACE, P, R, LEFT, RIGHT)
//    bool keysTriggered[GLFW_KEY_LAST + 1] = { false };
//
//    while (!glfwWindowShouldClose(window)) {
//    
//
//        double currentTime = glfwGetTime();
//
//        // Auto-drop (gravity)
//        if (!state.m_GameOver && !state.m_Paused) {
//            if ((currentTime - state.m_LastDropTime) * 1000.0f > state.m_DropInterval) {
//                state.MoveDown();
//                state.m_LastDropTime = currentTime;
//            }
//        }
//
//        // Continuous soft drop: DOWN fires every frame while held
//        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//            if (!state.m_GameOver && !state.m_Paused) {
//                state.MoveDown();
//                state.m_LastDropTime = currentTime;
//            }
//        }
//
//        // One-shot keys: trigger once per press
//        for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
//            bool pressed = (glfwGetKey(window, i) == GLFW_PRESS);
//
//            if (pressed && !keysTriggered[i]) {
//                if (i == GLFW_KEY_LEFT) { state.MoveLeft(); }
//                else if (i == GLFW_KEY_RIGHT) { state.MoveRight(); }
//                else if (i == GLFW_KEY_UP) { state.Rotate(); }
//                else if (i == GLFW_KEY_SPACE) { state.HardDrop(); }
//                else if (i == GLFW_KEY_P) { state.m_Paused = !state.m_Paused; }
//                else if (i == GLFW_KEY_R) {
//                    if (state.m_GameOver) {
//                        state.Init();
//                        state.m_LastDropTime = currentTime;
//                    }
//                }
//                keysTriggered[i] = true;
//            }
//
//            if (!pressed) {
//                keysTriggered[i] = false; // reset so next press can fire
//            }
//        }
//
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // Aspect-correct projection
//        int w, h;
//        glfwGetWindowSize(window, &w, &h);
//        float aspect = (float)w / (float)h;
//        float boardAspect = (float)BOARD_WIDTH / (float)BOARD_HEIGHT;
//
//        float halfW, halfH;
//        if (aspect > boardAspect) {
//            halfH = BOARD_HEIGHT / 2.0f + 1.0f;
//            halfW = halfH * aspect;
//        }
//        else {
//            halfW = BOARD_WIDTH / 2.0f + 1.0f;
//            halfH = halfW / aspect;
//        }
//
//        Matrix4 mvp = Matrix4::ortho(
//            -halfW, halfW,
//            -halfH, halfH,
//            -1.0f, 1.0f
//        );
//
//        if (renderer.shader) {
//            renderer.shader->use();
//            renderer.shader->setMat4("uMVP", mvp.m);
//        }
//
//        // Pure render: reads state, produces vertices
//        renderer.render(state, w, h);
//
//        // Render imgui overlay (score, level, speed)
//        {
//            ImGui::SetNextWindowPos(ImVec2(10, 10));
//            ImGui::SetNextWindowSize(ImVec2(200, 120));
//            ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
//            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SCORE: %d", state.m_Score);
//            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "LEVEL: %d", state.m_Level);
//
//            // Calculate speed (higher = faster)
//            int speed = (state.m_DropInterval > 0) ? (1000 / state.m_DropInterval) : 10;
//            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "SPEED: %d", speed);
//
//            // Show game state
//            if (state.m_GameOver) {
//                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "GAME OVER");
//                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press R to restart");
//            }
//            if (state.m_Paused) {
//                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "PAUSED");
//                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press P to resume");
//            }
//            ImGui::End();
//        }
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // Cleanup
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//
//    glfwTerminate();
//    return 0;
//}

