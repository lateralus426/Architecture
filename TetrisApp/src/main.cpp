#include<iostream>
#include "Core/Application.h"
#include "TetrisAppLayer.h"

int main() 
{
    Core::ApplicationSpecification appSpec;
    appSpec.Name = "TetrisGL";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Core::Application application(appSpec);
	application.PushLayer<TetrisAppLayer>();
    application.Run();

    std::cout << "Hello, Tetris!" << std::endl;

    return 0;
}