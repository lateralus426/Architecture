#include <iostream>
#include "GUILayer.h"
#include "Models.h"
#include "Core/Application.h"
#include "Core/Renderer/Renderer.h"

static constexpr const char* GetImGuiGlslVersion()
{
    std::cout << "[Iamgui] Opengl Version Major:" << OPENGL_VERSION_MAJOR << " Minor :" << OPENGL_VERSION_MINOR << "\n"; 
#if OPENGL_VERSION_MAJOR >= 4
    return "#version 460"; 
#elif OPENGL_VERSION_MAJOR == 3 && OPENGL_VERSION_MINOR >= 3
    return "#version 330";
#elif OPENGL_VERSION_MAJOR == 3 && OPENGL_VERSION_MINOR >= 2
    return "#version 150";
#else
    return "#version 130";
#endif
}





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

	m_GameState = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(Core::Application::Get().GetWindow()->GetHandle(), true);
    // Setup Platform/Renderer bindings
#if defined(USE_OPENGL) && OPENGL_VERSION_MAJOR >= 2
    const char* glsl_version = GetImGuiGlslVersion();
	std::cout << "GUILayer::GUILayer() glsl_version:" << glsl_version << "\n";

    ImGui_ImplOpenGL3_Init(glsl_version);
#elif defined(USE_OPENGL) && OPENGL_VERSION_MAJOR == 1
    const char* glsl_version = GetImGuiGlslVersion();
    std::cout << "GUILayer::GUILayer() glsl_version:" << glsl_version << "\n";
    ImGui_ImplOpenGL2_Init();
    // 3. Reset legacy state before ImGui draw
    //glDisable(GL_LIGHTING);
    //glDisable(GL_COLOR_MATERIAL);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#endif

}

GUILayer::~GUILayer()
{
#if defined(USE_OPENGL) && OPENGL_VERSION_MAJOR >= 2
    ImGui_ImplOpenGL3_Shutdown();
#elif defined(USE_OPENGL) && OPENGL_VERSION_MAJOR == 1
    ImGui_ImplOpenGL2_Shutdown();
#endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void GUILayer::OnUpdate(float ts)
{
   
}

void GUILayer::OnRender()
{
    // Start the Dear ImGui frame
#if defined(USE_OPENGL) && OPENGL_VERSION_MAJOR >= 2
    ImGui_ImplOpenGL3_NewFrame();
#elif defined(USE_OPENGL) && OPENGL_VERSION_MAJOR == 1
    ImGui_ImplOpenGL2_NewFrame();
#endif  
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(200, 120));
    ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SCORE: %d", m_GameState->m_Score);
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "LEVEL: %d", m_GameState->m_Level);

    // Calculate speed (higher = faster)
    int speed = (m_GameState->m_DropInterval > 0) ? (1000 / m_GameState->m_DropInterval) : 10;
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "SPEED: %d", speed);

    // Show game state
    if (m_GameState->m_GameOver) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "GAME OVER");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press R to restart");
    }
    if (m_GameState->m_Paused) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "PAUSED");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "Press P to resume");
    }
    ImGui::End();
 

    ImGui::Render();
#if defined(USE_OPENGL) && OPENGL_VERSION_MAJOR >= 2
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#elif defined(USE_OPENGL) && OPENGL_VERSION_MAJOR == 1
    //ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(Core::Application::Get().GetWindow()->GetHandle(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
    //GLint last_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    //glUseProgram(0);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //auto renderData = ImGui::GetDrawData();
    //ImGui_ImplOpenGL2_RenderDrawData(renderData);
#endif

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
