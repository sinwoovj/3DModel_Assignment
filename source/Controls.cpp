#include "Controls.h"
#include "Level.h"
#include <GLFW/glfw3.h>

void Controls::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    //ESC - CLOSE WINDOW
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(pWindow, GL_TRUE);

    //SPACE - RELOAD SHADER
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_SPACE)
            Level::GetPtr()->ReloadShaderProgram();


    //TODO: ADD CAMERA CONTROLS
    //TODO: ADD/DECRESE SLICES
    //TODO: TRIGGER WIREFRAME
    //TODO: TRIGGER TEXTURE
    //TODO: TRIGGER NORMALS RENDER
    //TODO: TRIGGER NORMALS AVERAGE
}
