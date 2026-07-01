#include<iostream>
#include "Core/Application.h"
#include "TetrisAppLayer.h"
#include "GUILayer.h"

int main() 
{

#ifdef USE_OPENGL
    #if OPENGL_VERSION_MAJOR >= 4
        std::cout << "Use modern OpenGL 4.x rendering path\n";
    #elif OPENGL_VERSION_MAJOR == 3
        std::cout << "Use OpenGL 3.x rendering path\n";
    #else
        std::cout << "Use legacy OpenGL path\n";
    #endif
#endif

    Core::ApplicationSpecification appSpec;
    appSpec.Name = "TetrisGL";
    int width = 1024;
    int height = 768;
    appSpec.WindowSpec.Width = width;
    appSpec.WindowSpec.Height = height;

    Core::Application application(appSpec);
	application.PushLayer<TetrisAppLayer>();
    auto tetrisLayer = application.GetLayer<TetrisAppLayer>();
    if(tetrisLayer == nullptr){
        std::cout << "Invalid Tetris Layer\n";
        return 0;
    }
	application.PushLayer<GUILayer>();
    auto guiLayer = application.GetLayer<GUILayer>();
    if(guiLayer != nullptr){
        guiLayer->SetGameState(tetrisLayer->GetGameState());
    }
    application.Run();

    std::cout << "Hello, Tetris!" << std::endl;

    return 0;
}


