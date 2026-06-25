#include "GUILayer.h"


GUILayer::GUILayer()
{
    // Setup Dear ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init("#version 330");
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
