#include "Controls.h"
#include "Level.h"
#include <GLFW/glfw3.h>

void Controls::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    Level* ptr = Level::GetPtr();
    //ESC - CLOSE WINDOW
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(pWindow, GL_TRUE);

    //SPACE - RELOAD SHADER
    if (action == GLFW_PRESS)
        if (key == GLFW_KEY_SPACE)
            ptr->ReloadShaderProgram();

    //TODO: ADD CAMERA CONTROLS
    //W,A,S,D,Q,E - CAMERA CONTROL
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
        ptr->RotateCamY(1);
    else if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
        ptr->RotateCamY(-1);
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
        ptr->RotateCamX(-1);
    else if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
        ptr->RotateCamX(1);
    if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS)
        ptr->ZoomCamZ(-1);
    else if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)
        ptr->ZoomCamZ(1);
        
    //TODO: ADD/DECRESE SLICES
    //TODO: TRIGGER WIREFRAME
    //TODO: TRIGGER TEXTURE
    //TODO: TRIGGER NORMALS RENDER
    //TODO: TRIGGER NORMALS AVERAGE
}
