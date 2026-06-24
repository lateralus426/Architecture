#pragma once

#include "gl_loader.h"
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

enum class ColorId : int {
    EMPTY = 0,
    I = 1, O = 2, T = 3, S = 4, Z = 5, J = 6, L = 7
};

// Color lookup table for rendering
const std::array<float, 3> PIECE_COLORS[] = {
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
    PieceType currentType;
    std::vector<std::pair<int, int>> m_CurrentCells;
    int currentRow, currentCol;

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

    GameState() : m_Rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
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

    void Rotate();
    void Init();
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

class Renderer {
public:
    Shader* shader = nullptr;
    GLuint vao = 0, vbo = 0;

    Renderer() {}

    ~Renderer() {
        if (shader) delete shader;
        if (vbo != 0) glDeleteBuffers(1, &vbo);
#ifdef GL3_PLUS_SUPPORT
        if (vao != 0) glDeleteVertexArrays(1, &vao);
#endif
    }

    void init() {
#ifdef GL3_PLUS_SUPPORT
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
#else
        glGenBuffers(1, &vbo);
#endif
    }

    void setupShader(const char* vsPath, const char* fsPath) {
        shader = new Shader(vsPath, fsPath);
    }

    float worldX(int gx) { return gx - BOARD_WIDTH / 2.0f + CELL_SIZE / 2.0f; }
    float worldY(int gy) { return -gy + BOARD_HEIGHT / 2.0f - 0.5f; }

    // Edge position: grid line at column gx = gx - WIDTH/2 (exact edge, not center)
    float edgeX(int gx) { return gx - BOARD_WIDTH / 2.0f; }
    // Edge position: grid line at row gy = HEIGHT/2 - gy (Y inverted, exact edge)
    float edgeY(int gy) { return (BOARD_HEIGHT / 2.0f) - gy; }

    void addLineQuad(std::vector<Vertex>& verts, int gx1, int gy1, int gx2, int gy2, float r, float g, float b) {
        float x1 = edgeX(gx1);
        float x2 = edgeX(gx2);
        float y1 = edgeY(gy1);
        float y2 = edgeY(gy2);
        float thick = 0.02f;

        if (y1 == y2) {
            // Horizontal line
            verts.push_back({ x1, y1 - thick, 0.0f, r, g, b });
            verts.push_back({ x2, y1 - thick, 0.0f, r, g, b });
            verts.push_back({ x2, y1 + thick, 0.0f, r, g, b });
            verts.push_back({ x1, y1 - thick, 0.0f, r, g, b });
            verts.push_back({ x2, y1 + thick, 0.0f, r, g, b });
            verts.push_back({ x1, y1 + thick, 0.0f, r, g, b });
        }
        else {
            // Vertical line
            verts.push_back({ x1 - thick, y1, 0.0f, r, g, b });
            verts.push_back({ x1 + thick, y1, 0.0f, r, g, b });
            verts.push_back({ x1 + thick, y2, 0.0f, r, g, b });
            verts.push_back({ x1 - thick, y1, 0.0f, r, g, b });
            verts.push_back({ x1 + thick, y2, 0.0f, r, g, b });
            verts.push_back({ x1 - thick, y2, 0.0f, r, g, b });
        }
    }

    // Helper: push a quad for given grid coords
    void pushQuad(std::vector<Vertex>& verts, int x, int y, float r, float g, float b) {
        float wx = worldX(x);
        float wy = worldY(y);
        float inset = 0.05f;

        verts.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        verts.push_back({ wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        verts.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
        verts.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
        verts.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
        verts.push_back({ wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
    }

    // Next piece preview box: centered on the right side of the board
    void renderNextPiece(std::vector<Vertex>& verts, PieceType type, const std::vector<std::pair<int, int>>& cells) {
        // type is valid (I=0 through L=6), no EMPTY variant
        int idx = static_cast<int>(type);
        float r = PIECE_COLORS[idx][0];
        float g = PIECE_COLORS[idx][1];
        float b = PIECE_COLORS[idx][2];

        // Preview box dimensions
        int boxLeft = BOARD_WIDTH + 2;
        int boxRight = BOARD_WIDTH + 5;
        int boxTop = 6;   // top row
        int boxBottom = 16;  // bottom row

        // Background for preview box (drawn first)
        for (int y = boxTop; y < boxBottom; ++y) {
            for (int x = boxLeft; x < boxRight; ++x) {
                pushQuad(verts, x, y, 0.08f, 0.08f, 0.14f);
            }
        }

        // Border for preview box
        addLineQuad(verts, boxLeft, boxTop, boxLeft, boxBottom, 0.35f, 0.35f, 0.5f);
        addLineQuad(verts, boxRight, boxTop, boxRight, boxBottom, 0.35f, 0.35f, 0.5f);
        addLineQuad(verts, boxLeft, boxTop, boxRight, boxTop, 0.35f, 0.35f, 0.5f);
        addLineQuad(verts, boxLeft, boxBottom, boxRight, boxBottom, 0.35f, 0.35f, 0.5f);

        // Center the piece in the preview box
        int minR = 100, maxR = -1, minC = 100, maxC = -1;
        for (auto& [dr, dc] : cells) {
            minR = std::min(minR, dr); maxR = std::max(maxR, dr);
            minC = std::min(minC, dc); maxC = std::max(maxC, dc);
        }

        // Center position in preview box
        int centerX = (boxLeft + boxRight) / 2;
        int centerY = (boxTop + boxBottom) / 2;
        int offsetX = centerX - (minC + maxC) / 2;
        int offsetY = centerY - (minR + maxR) / 2;

        // Draw the piece
        for (auto& [dr, dc] : cells) {
            int rx = dc + offsetX;
            int ry = dr + offsetY;
            pushQuad(verts, rx, ry, r, g, b);
        }
    }

    void render(const GameState& state, int winW, int winH) {
        std::vector<Vertex> vertices;
        vertices.reserve(BOARD_WIDTH * BOARD_HEIGHT * VERTS_PER_QUAD * 3);

        auto addQuad = [&](int x, int y, float r, float g, float b) {
            float wx = worldX(x);
            float wy = worldY(y);
            float inset = 0.05f;

            vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
            vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
            vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });

            vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy - CELL_SIZE / 2 + inset, 0.0f, r, g, b });
            vertices.push_back({ wx + CELL_SIZE / 2 - inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
            vertices.push_back({ wx - CELL_SIZE / 2 + inset, wy + CELL_SIZE / 2 - inset, 0.0f, r, g, b });
            };

        // 1) Background (dark pit)
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            for (int x = 0; x < BOARD_WIDTH; ++x) {
                addQuad(x, y, 0.06f, 0.06f, 0.1f);
            }
        }

        // 2) Grid lines (on top of background)
        for (int x = 0; x <= BOARD_WIDTH; ++x) {
            addLineQuad(vertices, x, 0, x, BOARD_HEIGHT, 0.2f, 0.2f, 0.28f);
        }
        for (int y = 0; y <= BOARD_HEIGHT; ++y) {
            addLineQuad(vertices, 0, y, BOARD_WIDTH, y, 0.2f, 0.2f, 0.28f);
        }

        // 3) Placed pieces from grid
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            for (int x = 0; x < BOARD_WIDTH; ++x) {
                if (state.m_Grid[y][x] > 0) {
                    int idx = state.m_Grid[y][x] - 1;
                    addQuad(x, y,
                        PIECE_COLORS[idx][0],
                        PIECE_COLORS[idx][1],
                        PIECE_COLORS[idx][2]);
                }
            }
        }

        // 4) Outer border (matches grid edges exactly)
        addLineQuad(vertices, 0, 0, 0, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);
        addLineQuad(vertices, BOARD_WIDTH, 0, BOARD_WIDTH, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);
        addLineQuad(vertices, 0, 0, BOARD_WIDTH, 0, 0.45f, 0.45f, 0.6f);
        addLineQuad(vertices, 0, BOARD_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, 0.45f, 0.45f, 0.6f);

        // 5) Ghost piece (read-only computation)
        if (!state.m_GameOver) {
            int ghostRow = -1;
            for (int testRow = state.currentRow; testRow < BOARD_HEIGHT; ++testRow) {
                bool canDrop = true;
                for (auto& [dr, dc] : state.m_CurrentCells) {
                    int r = testRow + dr;
                    int c = state.currentCol + dc;
                    if (c < 0 || c >= BOARD_WIDTH) { canDrop = false; break; }
                    if (r >= BOARD_HEIGHT) { canDrop = false; break; }
                    if (r >= 0 && state.m_Grid[r][c] != 0) { canDrop = false; break; }
                }
                if (!canDrop) {
                    ghostRow = testRow - 1;
                    break;
                }
                if (testRow == BOARD_HEIGHT - 1) ghostRow = testRow;
            }

            if (ghostRow >= 0 && ghostRow != state.currentRow) {
                int idx = static_cast<int>(state.currentType);
                for (auto& [dr, dc] : state.m_CurrentCells) {
                    int r = ghostRow + dr;
                    int c = state.currentCol + dc;
                    if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH) {
                        addQuad(c, r,
                            PIECE_COLORS[idx][0] * 0.3f,
                            PIECE_COLORS[idx][1] * 0.3f,
                            PIECE_COLORS[idx][2] * 0.3f);
                    }
                }
            }
        }

        // 6) Current piece (on top)
        if (!state.m_GameOver) {
            int idx = static_cast<int>(state.currentType);
            for (auto& [dr, dc] : state.m_CurrentCells) {
                int r = state.currentRow + dr;
                int c = state.currentCol + dc;
                if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH) {
                    addQuad(c, r,
                        PIECE_COLORS[idx][0],
                        PIECE_COLORS[idx][1],
                        PIECE_COLORS[idx][2]);
                }
            }
        }

        // 7) Next piece preview (right side of screen)
        renderNextPiece(vertices, state.m_NextType, state.m_NextCells);

        // Draw
#ifdef GL3_PLUS_SUPPORT
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);
#else
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
    }
};

// ==================== Main ====================

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    int width = 1024;
    int height = 768;

#ifdef GL3_PLUS_SUPPORT
    std::cout << "Requesting OpenGL 3.3 Core Profile\n";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#else
    std::cout << "Requesting OpenGL 2.1 Legacy Profile\n";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, "Tetris OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int width, int height) {
        glViewport(0, 0, width, height);
        (void)win;
        });

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    Renderer renderer;
    renderer.init();

#ifdef GL3_PLUS_SUPPORT
    renderer.setupShader("shaders/modern_vs.glsl", "shaders/modern_fs.glsl");
#else
    renderer.setupShader("shaders/legacy_vs.glsl", "shaders/legacy_fs.glsl");
#endif
    std::cout << "Finished compiling the shaders\n";

    GameState state;
    state.Init();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // One-shot keys: trigger once per press (UP, SPACE, P, R, LEFT, RIGHT)
    bool keysTriggered[GLFW_KEY_LAST + 1] = { false };

    while (!glfwWindowShouldClose(window)) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double currentTime = glfwGetTime();

        // Auto-drop (gravity)
        if (!state.m_GameOver && !state.m_Paused) {
            if ((currentTime - state.m_LastDropTime) * 1000.0f > state.m_DropInterval) {
                state.MoveDown();
                state.m_LastDropTime = currentTime;
            }
        }

        // Continuous soft drop: DOWN fires every frame while held
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (!state.m_GameOver && !state.m_Paused) {
                state.MoveDown();
                state.m_LastDropTime = currentTime;
            }
        }

        // One-shot keys: trigger once per press
        for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
            bool pressed = (glfwGetKey(window, i) == GLFW_PRESS);

            if (pressed && !keysTriggered[i]) {
                if (i == GLFW_KEY_LEFT) { state.MoveLeft(); }
                else if (i == GLFW_KEY_RIGHT) { state.MoveRight(); }
                else if (i == GLFW_KEY_UP) { state.Rotate(); }
                else if (i == GLFW_KEY_SPACE) { state.HardDrop(); }
                else if (i == GLFW_KEY_P) { state.m_Paused = !state.m_Paused; }
                else if (i == GLFW_KEY_R) {
                    if (state.m_GameOver) {
                        state.Init();
                        state.m_LastDropTime = currentTime;
                    }
                }
                keysTriggered[i] = true;
            }

            if (!pressed) {
                keysTriggered[i] = false; // reset so next press can fire
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Aspect-correct projection
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        float aspect = (float)w / (float)h;
        float boardAspect = (float)BOARD_WIDTH / (float)BOARD_HEIGHT;

        float halfW, halfH;
        if (aspect > boardAspect) {
            halfH = BOARD_HEIGHT / 2.0f + 1.0f;
            halfW = halfH * aspect;
        }
        else {
            halfW = BOARD_WIDTH / 2.0f + 1.0f;
            halfH = halfW / aspect;
        }

        Matrix4 mvp = Matrix4::ortho(
            -halfW, halfW,
            -halfH, halfH,
            -1.0f, 1.0f
        );

        if (renderer.shader) {
            renderer.shader->use();
            renderer.shader->setMat4("uMVP", mvp.m);
        }

        // Pure render: reads state, produces vertices
        renderer.render(state, w, h);

        // Render imgui overlay (score, level, speed)
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::SetNextWindowSize(ImVec2(200, 120));
            ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SCORE: %d", state.m_Score);
            ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "LEVEL: %d", state.m_Level);

            // Calculate speed (higher = faster)
            int speed = (state.m_DropInterval > 0) ? (1000 / state.m_DropInterval) : 10;
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "SPEED: %d", speed);

            // Show game state
            if (state.m_GameOver) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "GAME OVER");
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press R to restart");
            }
            if (state.m_Paused) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "PAUSED");
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press P to resume");
            }
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

