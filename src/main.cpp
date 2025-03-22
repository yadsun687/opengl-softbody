#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/shader_s.h>

#include <math.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <GraphicEngine/GraphicEngine.h>
#include <PhysicEngine/PhysicsEngine.h>
#include <glm/glm.hpp>

GraphicEngine *g_engine;
PhysicsEngine *p_engine;

int main()
{
    g_engine = new GraphicEngine();
    p_engine = new PhysicsEngine();

    //use absolute path for now
    //change to your project path
    g_engine->text_renderer->loadFont("D:/CODE/ComGraphic/project-space/resources/fonts/OpenSans-Regular.ttf",24);

    p_engine->instanceBuffer = g_engine->getInstanceBuffer(); // shared instance buffer over engine
    p_engine->colorBuffer = g_engine->getColorBuffer();       // shared color buffer over engine

    // add cube at different position to PhysicEngine
    for (int i = -20; i < 20; i++)
    {
        for (int k = -20; k < 20; k++)
        {
            p_engine->addPhysicObject(
                CUBE,
                glm::vec3((float)i * 1.0f, (float)sin(i * k), (float)k * 1.0f),
                glm::vec4((float)(i % 10) * 0.1f, (float)((i * k) % 10) * 0.1f, (float)(k % 10) * 0.1f, 1.0f));
        }
    }

    // TODO: now have no data for mesh position in buffer >> only support 1 mesh
    for (int i = 0; i < cube.vertices.size(); i++)
    {
        g_engine->getVertexBuffer().push_back(cube.vertices[i]);
    }
    for (auto idx : cube.indices)
    {
        g_engine->getIndexBuffer().push_back(idx);
    }

    // call these whenever data in buffers get update
    g_engine->updateVertexVBO();
    g_engine->updateIndexEBO();
    g_engine->updateInstanceVBO();
    g_engine->updateColorVBO();

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
