#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <chrono>

#include <array>


// Camera data-structure 
struct Camera {
    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    glm::vec3 m_Up;

    // Camera parameters
    float m_Fov;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    glm::mat4x4 m_View;
    glm::mat4x4 m_Projection;
    glm::ivec2 m_ScreenSize;
};


// ==================== Compile-Time API Selection ====================
#define GL3_PLUS_SUPPORT

// ==================== Constants ====================

constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr float CELL_SIZE = 1.0f;
constexpr int VERTS_PER_QUAD = 6;
constexpr float QUAD_INSET = 0.05f;
constexpr float LINE_THICKNESS = 0.02f;



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


