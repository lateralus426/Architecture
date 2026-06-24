#pragma once

//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"  // Same file works for GL 3.3 AND ES 2.0/3.0

#include "Core/Application.h"
#include <glm/glm.hpp>



namespace Core {
	class MouseButtonPressedEvent;
	class MouseMovedEvent;
	class WindowClosedEvent;
};
class GUILayer : public Core::Layer
{
public:
	GUILayer();
	virtual ~GUILayer();

	virtual void OnEvent(Core::Event& event) override;

	virtual void OnUpdate(float ts) override;
	virtual void OnRender() override;
private:
	bool OnMouseButtonPressed(Core::MouseButtonPressedEvent& event);
	bool OnMouseMoved(Core::MouseMovedEvent& event);
	bool OnWindowClosed(Core::WindowClosedEvent& event);
private:
	
};