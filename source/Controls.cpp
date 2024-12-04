#include "Controls.h"
#include "Level.h"
#include <GLFW/glfw3.h>
#include "models.h"

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

    //ptr->key.init(GLFW_RELEASE);
    //TODO: ADD CAMERA CONTROLS / W,A,S,D,Q,E - CAMERA CONTROL
    //TODO: ADD/DECRESE SLICES
    //TODO: TRIGGER WIREFRAME
    //TODO: TRIGGER TEXTURE
    //TODO: TRIGGER NORMALS RENDER
    //TODO: TRIGGER NORMALS AVERAGE
    if (key == GLFW_KEY_A)
        ptr->key.A = action;
    if (key == GLFW_KEY_D)
        ptr->key.D = action;
    if (key == GLFW_KEY_W)
        ptr->key.W = action;
    if (key == GLFW_KEY_S)
        ptr->key.S = action;
    if (key == GLFW_KEY_Q)
        ptr->key.Q = action;
    if (key == GLFW_KEY_E)
        ptr->key.E = action;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_M)
        {
            static bool polyState = false;
            polyState = !polyState;
            glPolygonMode(GL_FRONT_AND_BACK, polyState ? GL_LINE : GL_FILL);
        }
        if (key == GLFW_KEY_Z || key == GLFW_KEY_KP_SUBTRACT)
        {
            if (Model::slices > 4) Model::slices--;
            for (auto o : ptr->allObjects)
            {
                o->InitModel();
            }
        }
        if (key == GLFW_KEY_X || key == GLFW_KEY_KP_ADD)
        {
            Model::slices++;
            for (auto o : ptr->allObjects)
            {
                o->InitModel();
            }
        }
        if (key == GLFW_KEY_N)
            ptr->showNormal = !ptr->showNormal;
        /*if (key  == GLFW_KEY_T)
            ptr->key.T = action;*/
        if (key == GLFW_KEY_F)
        {
            ptr->normalAvg = !ptr->normalAvg;
            for (auto o : ptr->allObjects)
            {
                o->InitModel();
            }
        }
    }
}
