#pragma once


#include "Core/Application.h"
#include <glm/glm.hpp>
#include "Models.h"


namespace Core {
    class MouseButtonPressedEvent;
    class MouseMovedEvent;
    class WindowClosedEvent;
};



class CubeLayer : public Core::Layer
{
public:
    CubeLayer();
    virtual ~CubeLayer();

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
    struct GameState m_GameState;

    Camera m_Camera;

public:
    
    // cube 
    void InitializeCube();
    void UpdateCube();
    void RenderCubeVertices();

};
