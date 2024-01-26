#include <iostream>
#include <fstream>
#include <vector>
#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "project.hpp"


int main(int argc, char** argv)
{
    // inicjalizacja glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // tworzenie okna za pomocą glfw
    GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain Rendering", nullptr, nullptr);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ladowanie OpenGL za pomoca glew
    glewInit();
    glViewport(0, 0, 800, 600);

    init(window);

    // uruchomienie glownej petli
    renderLoop(window);

    shutdown(window);
    glfwTerminate();
    return 0;
}
