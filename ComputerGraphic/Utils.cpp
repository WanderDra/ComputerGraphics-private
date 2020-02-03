#include "Utils.h"

using namespace std;

void *Utils::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    return NULL;
}

