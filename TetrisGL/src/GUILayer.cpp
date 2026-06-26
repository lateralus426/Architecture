#include <iostream>
#include "GUILayer.h"



#ifdef USE_OPENGL
    //#include <glad/glad.h>  // Initialize with gladLoadGL()
    //#include "imgui_impl_glfw.h"
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

#if OPENGL_VERSION GREATER_EQUAL 3
#include "imgui_impl_opengl3.h"
#elif OPENGL_VERSION STREQUAL "2"
#include "imgui_impl_opengl2.h"
#endif

GUILayer::GUILayer()
{
    // Setup Dear ImGui context

    std::cout << "GUILayer::GUILayer()\n";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    // (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
#if defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init("#version 330");
#endif
}

GUILayer::~GUILayer()
{
}


void GUILayer::OnUpdate(float ts)
{
    /*     ImGui::SetNextWindowPos(ImVec2(10, 10));
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
  */
}

void GUILayer::OnRender()
{
    // Start the Dear ImGui frame
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();
   
 

   /* ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/

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
