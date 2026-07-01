#pragma once

#include "Core/Application.h"
#include <glm/glm.hpp>
#include "Models.h"


namespace Core {
	class MouseButtonPressedEvent;
	class MouseMovedEvent;
	class WindowClosedEvent;
};

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
    glm::ivec2 ScreenSize;
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
    uint32_t m_VertexArray = 0;
    uint32_t m_VertexBuffer = 0;  // handle to the vertex buffer object (VBO)
    // TODO: change to uint32_t
    uint32_t m_PosBuffer = 0;
    uint32_t m_ColorBuffer = 0;
    uint32_t m_IndexBuffer = 0;

    float m_Time = 0.0f;
    float m_Angle = 0.0f;
	float m_Speed = 90.0f; // degrees per second
    float m_CurrentTime = 0.0f;
    glm::vec2 m_MousePosition{ 0.0f };
    glm::vec2 m_FlamePosition{ 0.0f };
    struct GameState m_GameState;

	Camera m_Camera;

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
  /*

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

*/
   // glfwMakeContextCurrent(window);
   // glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int width, int height) {
   //     glViewport(0, 0, width, height);
   //     (void)win;
   //     });

    //glewExperimental = GL_TRUE;
    //if (glewInit() != GLEW_OK) {
    //    std::cerr << "Failed to initialize GLEW" << std::endl;
    //    return -1;
    //}

    //glViewport(0, 0, width, height);
    //glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
//
    //Renderer renderer;
    //renderer.init();
//

   