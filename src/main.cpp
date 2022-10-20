#include "Application.h"

using namespace GLMV;

int main()
{
    Application* app = new Application("OpenGL Model Viewer");
    app->Run();
    delete app;
}
