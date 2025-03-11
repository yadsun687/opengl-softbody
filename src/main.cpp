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
PhysicsEngine *p_engine = nullptr;

int main()
{
    g_engine = new GraphicEngine();
    p_engine = new PhysicsEngine();

    p_engine->instanceBuffer = g_engine->getInstanceBuffer();

    for (int i = -10; i < 100; i++)
    {
        for (int j = -10; j < 10; j++)
        {
            for (int k = -10; k < 100; k++)
            {
                p_engine->addPhysicObject(CUBE, glm::vec3((float)i * 2.0f, (float)j * 2.0f, (float)k * 2.0f));
            }
        }
    }
    // p_engine->addPhysicObject(CUBE, glm::vec3((float)1 * 2.0f, (float)1 * 2.0f, (float)1 * 2.0f));


    for (int i = 0; i < cube.vertices.size(); i++)
    {
        g_engine->getVertexBuffer().push_back(cube.vertices[i]);

        if (i % 3 == 2)
        {
            auto endptr = g_engine->getVertexBuffer().end();
            g_engine->getVertexBuffer().insert(endptr, {1.0f, 0.6f, 0.0f, 1.0f});
        }
    }

    for (auto idx : cube.indices)
    {
        g_engine->getIndexBuffer().push_back(idx);
    }

    g_engine->updateVertexVBO();
    g_engine->updateIndexEBO();
    g_engine->updateinstanceVBO();

    // for (auto &i : *g_engine->getInstanceBuffer())
    // {
    //     std::cout << i.x << "," << i.y << "," << i.z << "\n";
    // }

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
