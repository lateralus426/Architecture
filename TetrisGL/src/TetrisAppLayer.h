#pragma once

#include "Core/Application.h"
#include <glm/glm.hpp>



namespace Core {
	class MouseButtonPressedEvent;
	class MouseMovedEvent;
	class WindowClosedEvent;
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

	float m_Time = 0.0f;
	float m_Angle = 0.0f;
	glm::vec2 m_MousePosition{ 0.0f };
	glm::vec2 m_FlamePosition{ 0.0f };

};