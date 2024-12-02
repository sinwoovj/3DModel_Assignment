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

    ptr->key.init(GLFW_RELEASE);
    //TODO: ADD CAMERA CONTROLS / W,A,S,D,Q,E - CAMERA CONTROL
    //TODO: ADD/DECRESE SLICES
    //TODO: TRIGGER WIREFRAME
    //TODO: TRIGGER TEXTURE
    //TODO: TRIGGER NORMALS RENDER
    //TODO: TRIGGER NORMALS AVERAGE
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_A)
            ptr->key.A = GLFW_PRESS;
        if (key == GLFW_KEY_D)
            ptr->key.D = GLFW_PRESS;
        if (key == GLFW_KEY_W)
            ptr->key.W = GLFW_PRESS;
        if (key == GLFW_KEY_S)
            ptr->key.S = GLFW_PRESS;
        if (key == GLFW_KEY_Q)
            ptr->key.Q = GLFW_PRESS;
        if (key == GLFW_KEY_E)
            ptr->key.E = GLFW_PRESS;
        if (key == GLFW_KEY_N)
            ptr->key.N = GLFW_PRESS;
        if (key == GLFW_KEY_T)
            ptr->key.T = GLFW_PRESS;
        if (key == GLFW_KEY_F)
            ptr->key.F = GLFW_PRESS;
        if (key == GLFW_KEY_M)
            ptr->key.M = GLFW_PRESS;
        if (key == GLFW_KEY_Z)
            ptr->key.Z = GLFW_PRESS;
        if (key == GLFW_KEY_X)
            ptr->key.X = GLFW_PRESS;
    }
        
    
}
