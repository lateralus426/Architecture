#include<iostream>
#include "Core/Application.h"
#include "TetrisAppLayer.h"

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
    application.Run();

    std::cout << "Hello, Tetris!" << std::endl;

    return 0;
}


