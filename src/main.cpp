#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/shader_s.h>

#include <math.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <GraphicEngine/GraphicEngine.h>
#include <glm/glm.hpp>

GraphicEngine *engine;

int main()
{
    engine = new GraphicEngine();

    while(!engine->engineWindowShouldClose()){

        engine->engineUpdate(
            []()->void {
                engine->engineClearWindow(glm::vec4(1.0f,1.0f,0.0f,1.0f));

            }
        );

    }
}
