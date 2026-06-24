#include "GUILayer.h"


GUILayer::GUILayer()
{
}

GUILayer::~GUILayer()
{
}


void GUILayer::OnUpdate(float ts)
{
}

void GUILayer::OnRender()
{
}

void GUILayer::OnEvent(Core::Event& event)
{
}

bool GUILayer::OnMouseButtonPressed(Core::MouseButtonPressedEvent& event)
{
	return false;
}

bool GUILayer::OnMouseMoved(Core::MouseMovedEvent& event)
{
	return false;
}

bool GUILayer::OnWindowClosed(Core::WindowClosedEvent& event)
{
	return false;
}
