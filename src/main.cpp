#include <math.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Graphic/shader_s.h>
#include <Graphic/GraphicEngine.h>
#include <Physics/PhysicsEngine.h>
#include <Graphic/GUIManager.h>

GraphicEngine *g_engine;
PhysicsEngine *p_engine;
const float BOX_WIDTH = 30.0f;
const float BOX_DEPTH = 30.0f;
const float BOX_HEIGHT = 30.0f;

// USER INPUT SHORTCUT HERE
static void shortcutCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (g_engine)
    {
        // left alt to show/hide cursor
        if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE)
        {
            g_engine->cursor_hidden = !g_engine->cursor_hidden;
            glfwSetInputMode(window, GLFW_CURSOR, g_engine->cursor_hidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            glfwSetCursorPos(g_engine->window, g_engine->lastX, g_engine->lastY);
        }
        // reset simulation
        if (key == GLFW_KEY_R && action == GLFW_RELEASE)
        {
            p_engine->sph_solver->resetSimulation();
            g_engine->updateSolverBuffer();
        }
        // pause simulation
        if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        {
            p_engine->is_pause = !p_engine->is_pause;
        }

        // 1 step physics update
        if (key == GLFW_KEY_U && action == GLFW_RELEASE)
        {   
            p_engine->update(g_engine->deltaTime , true);
        }
    }
}

int main()
{
    g_engine = new GraphicEngine(); // also init GUIManager inside
    glfwSetKeyCallback(g_engine->window, shortcutCallback);
    // TODO: fix pathing
    g_engine->text_renderer->loadFont("D:/CODE/ComGraphic/project-space/resources/fonts/OpenSans-Regular.ttf", 24);

    p_engine = new PhysicsEngine();

    // // call these whenever data in buffers get update
    // g_engine->updateVertexVBO();
    // g_engine->updateIndexEBO();
    // g_engine->updateInstanceVBO();
    // g_engine->updateColorVBO();

    g_engine->setContainer(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH, glm::vec3(0.0f, 0.8f, 0.0f));
    p_engine->setBoundingBox(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH, glm::vec3(0.0f));

    p_engine->initSPH();                       // init simulation
    g_engine->bindWithPhysicsEngine(p_engine); // setup all buffer to render + simulation GUI

    // render loop
    while (!g_engine->engineWindowShouldClose())
    {
        g_engine->engineUpdate(
            [](float deltaTime) -> void
            {
                g_engine->engineClearWindow(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

                p_engine->update(deltaTime);
            });
    }
}
