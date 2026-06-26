#include<iostream>
#include "Core/Application.h"
#include "TetrisAppLayer.h"
#include "GUILayer.h"

int main() 
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "TetrisGL";
    int width = 1024;
    int height = 768;
    appSpec.WindowSpec.Width = width;
    appSpec.WindowSpec.Height = height;

    Core::Application application(appSpec);
	application.PushLayer<TetrisAppLayer>();
	application.PushLayer<GUILayer>();
    application.Run();

    std::cout << "Hello, Tetris!" << std::endl;

    return 0;
}


