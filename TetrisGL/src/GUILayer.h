#pragma once

//#include "Core/Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"  // Same file works for GL 3.3 AND ES 2.0/3.0
#if defined(USE_OPENGL) && OPENGL_VERSION_MAJOR >= 2
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#elif defined(USE_OPENGL) && OPENGL_VERSION_MAJOR == 1
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#elif defined(USE_VULKAN)
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#elif defined(USE_DX9)
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#elif defined(USE_DX10)
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#elif defined(USE_DX11)
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#elif defined(USE_DX12)
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#endif
#include "Core/Application.h"
#include <glm/glm.hpp>
//#include "imgui.h"


namespace Core {
	class MouseButtonPressedEvent;
	class MouseMovedEvent;
	class WindowClosedEvent;
};

struct GameState;

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

public:
	void SetGameState(const GameState& gameState) {
		// Store a reference to the game state
		m_GameState = &gameState;
	}

private:
	const GameState* m_GameState; 	
};