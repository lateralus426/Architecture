#pragma once

#include "Core/Application.h"
#include <glm/glm.hpp>
#include "Models.h"


namespace Core {
	class MouseButtonPressedEvent;
	class MouseMovedEvent;
	class WindowClosedEvent;
};


struct VertexBuffer {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void reserve(size_t quadCount) {
        vertices.reserve(quadCount * 4);
        indices.reserve(quadCount * 6);
    }

    void clear() {
        vertices.clear();
        indices.clear();
    }
};

class TetrisAppLayer : public Core::Layer
{
public:
    TetrisAppLayer();
    virtual ~TetrisAppLayer();

    virtual void OnEvent(Core::Event& event) override;

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
private:
    bool OnMouseButtonPressed(Core::MouseButtonPressedEvent& event);
    bool OnMouseMoved(Core::MouseMovedEvent& event);
    bool OnWindowClosed(Core::WindowClosedEvent& event);
private:
    uint32_t m_Shader = 0;
    uint32_t m_VertexArray = 0;   // VAO
    uint32_t m_VertexBuffer = 0;  // handle to the vertex buffer object (VBO)
   
    uint32_t m_PosBuffer = 0;
    uint32_t m_ColorBuffer = 0;
    uint32_t m_IndexBuffer = 0;

	bool m_Initialized = false;

    float m_Time = 0.0f;
    float m_Angle = 0.0f;
	float m_Speed = 90.0f; // degrees per second
    float m_CurrentTime = 0.0f;
    glm::vec2 m_MousePosition{ 0.0f };
    glm::vec2 m_FlamePosition{ 0.0f };
    struct GameState m_GameState;
    class GLFWwindow* m_WindowHandle;
	Camera m_Camera;
    std::array<bool, GLFW_KEY_LAST + 1> m_KeysTriggered{ false };

public:
    const GameState& GetGameState() const { return m_GameState;}
    inline float WorldX(int gx) { return gx - BOARD_WIDTH / 2.0f + CELL_SIZE / 2.0f; }
    inline float WorldY(int gy) { return -gy + BOARD_HEIGHT / 2.0f - 0.5f; }

    // Edge position: grid line at column gx = gx - WIDTH/2 (exact edge, not center)
    inline float EdgeX(int gx) { return gx - BOARD_WIDTH / 2.0f; }
    // Edge position: grid line at row gy = HEIGHT/2 - gy (Y inverted, exact edge)
    inline float EdgeY(int gy) { return (BOARD_HEIGHT / 2.0f) - gy; }

    void AddLineQuad(std::vector<Vertex>& verts, int gx1, int gy1, int gx2, int gy2, float r, float g, float b);
    // Helper: push a quad for given grid coords
    void PushQuad(std::vector<Vertex>& verts, int x, int y, float r, float g, float b);

    void RenderNextPiece(std::vector<Vertex>& verts, PieceType type, const std::vector<std::pair<int, int>>& cells);

    void Render(const GameState& state, int winW, int winH);
    void Init();

    // cube 
    void InitializeCube();
    void InitializeCubeV1();
    void UpdateCube();
    void RenderCubeVertices();

};
 

   