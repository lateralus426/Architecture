#include "Models.h"

void GameState::ResetGrid() 
{
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            m_Grid[y][x] = 0;
        }
    }
}

PieceType GameState::RandomPieceType() {
    std::uniform_int_distribution<int> dist(0, 6);
    return static_cast<PieceType>(dist(m_Rng));
}

void GameState::LoadPiece(PieceType type) {
    m_CurrentType = type;
    m_CurrentCells = BASE_CELLS[static_cast<int>(type)];
    m_CurrentRow = 0;
    m_CurrentCol = BOARD_WIDTH / 2 - 1;
}


void GameState::LoadNextPiece() {
    m_NextType = RandomPieceType();
    m_NextCells = BASE_CELLS[static_cast<int>(m_NextType)];
}

bool GameState::IsValidPosition(int row, int col, const std::vector<std::pair<int, int>>& cells) const {
    for (auto& [dr, dc] : cells) {
        int r = row + dr;
        int c = col + dc;

        if (c < 0 || c >= BOARD_WIDTH) return false;
        if (r >= BOARD_HEIGHT) return false;

        if (r >= 0 && m_Grid[r][c] != 0) {
            return false;
        }
    }
    return true;
}

void GameState::PlacePiece() {
    for (auto& [dr, dc] : m_CurrentCells) {
        int r = m_CurrentRow + dr;
        int c = m_CurrentCol + dc;
        if (r >= 0 && r < BOARD_HEIGHT && c >= 0 && c < BOARD_WIDTH) {
            m_Grid[r][c] = static_cast<int>(m_CurrentType) + 1;
        }
    }
}

void GameState::ClearLines() {
    int lines = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; ++x) {
            if (m_Grid[y][x] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            lines++;
            for (int row = y; row > 0; --row) {
                for (int x = 0; x < BOARD_WIDTH; ++x) {
                    m_Grid[row][x] = m_Grid[row - 1][x];
                }
            }
            for (int x = 0; x < BOARD_WIDTH; ++x) {
                m_Grid[0][x] = 0;
            }
            y++;
        }
    }

    if (lines > 0) {
        int points[] = { 0, 100, 300, 500, 800 };
        m_Score += points[lines] * m_Level;
        m_LinesCleared += lines;
        m_Level = m_LinesCleared / 10 + 1;
        m_DropInterval = std::max(100, 1000 - (m_Level - 1) * 100);
    }
}

void GameState::MoveDown() {
    if (m_GameOver || m_Paused) return;
    if (!IsValidPosition(m_CurrentRow + 1, m_CurrentCol, m_CurrentCells)) {
        PlacePiece();
        ClearLines();
        LoadPiece(m_NextType);
        LoadNextPiece();
    }
    else {
        m_CurrentRow++;
    }
}

void GameState::MoveLeft() 
{
    if (m_GameOver || m_Paused) 
        return;
    
    if (IsValidPosition(m_CurrentRow, m_CurrentCol - 1, m_CurrentCells)) 
    {
        m_CurrentCol--;
    }
}

void GameState::MoveRight() 
{
    if (m_GameOver || m_Paused) 
        return;
    
    if (IsValidPosition(m_CurrentRow, m_CurrentCol + 1, m_CurrentCells))
    {
        m_CurrentCol++;
    }
}

void GameState::Rotate() {
    if (m_GameOver || m_Paused) return;

    // O piece is a square — skip
    if (m_CurrentType == PieceType::O) return;

    std::vector<std::pair<int, int>> original = m_CurrentCells;
    int originalRow = m_CurrentRow;
    int originalCol = m_CurrentCol;

    std::vector<std::pair<int, int>> rotated;

    int pivotR = 0, pivotC = 0;

    if (m_CurrentType == PieceType::I) {
        pivotR = 1; pivotC = 0;
        if (m_iRotationToggle % 2 == 0) {
            // Counter-clockwise
            for (auto& [dr, dc] : m_CurrentCells) {
                int nr = pivotR - (dc - pivotC);
                int nc = pivotC + (dr - pivotR);
                rotated.push_back({ nr, nc });
            }
        }
        else {
            // Clockwise
            for (auto& [dr, dc] : m_CurrentCells) {
                int nr = pivotR + (dc - pivotC);
                int nc = pivotC - (dr - pivotR);
                rotated.push_back({ nr, nc });
            }
        }
        m_iRotationToggle++;
    }
    else {
        pivotR = 1; pivotC = 1;
        for (auto& [dr, dc] : m_CurrentCells) {
            int nr = pivotR - (dc - pivotC);
            int nc = pivotC + (dr - pivotR);
            rotated.push_back({ nr, nc });
        }
    }

    // Wall kicks
    int colOffsets[] = { 0, 1, -1, 2, -2 };
    int rowOffsets[] = { 0, 1, -1, 2, -2 };
    for (int rOff : rowOffsets) {
        for (int cOff : colOffsets) {
            m_CurrentCells = rotated;
            if (IsValidPosition(originalRow + rOff, originalCol + cOff, m_CurrentCells)) {
                m_CurrentRow = originalRow + rOff;
                m_CurrentCol = originalCol + cOff;
                return;
            }
        }
    }

    m_CurrentCells = original;
    m_CurrentRow = originalRow;
    m_CurrentCol = originalCol;
}

void GameState::HardDrop() {
    if (m_GameOver || m_Paused) return;
    while (IsValidPosition(m_CurrentRow + 1, m_CurrentCol, m_CurrentCells)) {
        m_CurrentRow++;
    }
    PlacePiece();
    ClearLines();
    LoadPiece(m_NextType);
    LoadNextPiece();
}

void GameState::Init() {
    ResetGrid();
    m_Score = 0;
    m_Level = 1;
    m_LinesCleared = 0;
    m_DropInterval = 1000;
    m_iRotationToggle = 0;
    m_GameOver = false;
    m_Paused = false;

    LoadNextPiece();
    LoadPiece(m_NextType);
    LoadNextPiece();
}
