#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <shader/shader_s.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include <GraphicObject/BaseObject.h>
#include <GraphicEngine/Camera.h>

class GraphicEngine
{
public:
    GraphicEngine()
    {

        // init class member
        camera = Camera();
        projection = glm::mat4(1.0f);

        //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
        float offset = 0.5f;
        for (int y = -10; y < 10; y += 2)
        {
            for (int x = -10; x < 10; x += 2)
            {
                for (int z = -10; z < 10; z += 2)
                {
                    // create 1000 cube with different position
                    BaseObject c(CUBE, glm::vec4(0.2f, 1.0f, 0.5, 1.0f));
                    // BaseObject c(CUBE , glm::vec4(3.0f , 3.0f , 3.0f , 1.0f) );
                    c.translate(glm::vec3((float)x + offset, (float)y + offset, float(z) + offset));
                    cubeLists.push_back(c);
                    // also append model matrix to vertex,model buffer
                    cubeModelInstance.push_back(c.localOrigin); // location 2
                }
            }
        }
        cubeLists[0].loadToBuffer(vertexLists); // location 0,1
        //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]

        deltaTime = glfwGetTime();

        std::cout << "Initializing Graphic Engine...\n";

        initWindow(SCR_WIDTH, SCR_HEIGHT, "Particle simulation");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        shader = new Shader("src/shader.vs", "src/shader.fs");
        initBuffer();

        std::cout << "Graphic Engine start running...\n";
    }

    ~GraphicEngine();

    // initialize program window
    int initWindow(int width, int height, const char *title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, "TestWindow", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        return 0;
    }

    // check if window still running
    bool engineWindowShouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    void engineClearWindow(glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Initialize all buffer object
    //------------------
    // setup rendering configs
    void initBuffer()
    {
        shader->use();

        // apply projection matrix
        projection = glm::perspective(glm::radians(53.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader->setMat4("projection", projection);

        // z-axis drawing order
        glEnable(GL_DEPTH_TEST);

        glGenVertexArrays(1, &VAO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_DYNAMIC_DRAW);

        // using index to draw instead
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeLists[0].mesh_data.indices.size() * sizeof(unsigned int), cubeLists[0].mesh_data.indices.data(), GL_DYNAMIC_DRAW);

        // define data format stored in vertex buffers
        // [attribute No.0 | 3 elements (x,y,z) | float type | no normalized | total size | offset 0]
        // first 3 for vertex xyz, next 4 for color rgba
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
        glGenBuffers(1, &instanceVBO); // begin config instanceVBO
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // transformation matrix buffer for each cube
        // we will change each object position through this "cubeModelInstance" array
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * cubeModelInstance.size(), cubeModelInstance.data(), GL_DYNAMIC_DRAW);

        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(1 * vec4Size));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }

    void engineUpdate(void (*user_callback)())
    {
        // compute delta time
        float current_time = static_cast<float>(glfwGetTime());
        deltaTime = current_time - prevTime;
        prevTime = current_time;

        // input detect
        processInput();
        processMouse();

        shader->use();

        // custom callback
        user_callback();

        glBindVertexArray(VAO);

        // camera view update
        shader->setMat4("view", camera.GetViewMatrix());

        // glDrawArrays(GL_TRIANGLES, 0 , cubeVertList.size()/3 ); //draw using vertex coord
        // glDrawElements(GL_TRIANGLES, cube->mesh_data.indices.size(), GL_UNSIGNED_INT, 0); // draw using vertex indices
        glDrawElementsInstanced(GL_TRIANGLES, cubeLists[0].mesh_data.indices.size(), GL_UNSIGNED_INT, 0, cubeLists.size()); // draw instances

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

private:
    const int SCR_WIDTH = 800;
    const int SCR_HEIGHT = 480;

    GLFWwindow *window = nullptr;
    Shader *shader;
    unsigned int VBO, VAO, instanceVBO, EBO; // buffer
    float deltaTime, prevTime;               // for frame independent rendering

    Camera camera;

    std::vector<float> vertexLists;
    std::vector<unsigned int> indexLists;
    BaseObject *cube;

    //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
    std::vector<BaseObject> cubeLists;
    std::vector<glm::mat4> cubeModelInstance;
    //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]

    // camera
    bool firstMouse;
    float lastX;
    float lastY;

    // transformation matrix for vertex shader
    glm::mat4 view;
    glm::mat4 projection;

    //======================[PRIVATE METHOD]========================
    void processInput()
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // camera movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::ASCEND, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(Camera_Movement::DESCEND, deltaTime);
    }

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    void processMouse()
    {

        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        xpos = static_cast<float>(xpos);
        ypos = static_cast<float>(ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
};
