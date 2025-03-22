#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <shader/shader_s.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <GraphicEngine/TextRenderer.h>
#include <PhysicEngine/PhysicsObject.h>
#include <GraphicEngine/Camera.h>

class GraphicEngine
{
public:
    GraphicEngine()
    {

        // init class member
        camera = Camera();
        projection = glm::mat4(1.0f);

        vertexLists = std::vector<float>();
        indexLists = std::vector<unsigned int>();
        modelInstance = std::vector<glm::vec3>();

        deltaTime = glfwGetTime();

        std::cout << "Initializing Graphic Engine...\n";

        initWindow(SCR_WIDTH, SCR_HEIGHT, "Particle simulation");

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        // init shader after init glfw
        // use absolute path for now
        // change to your project path
        shader = new Shader("D:/CODE/ComGraphic/project-space/src/shader.vs", "D:/CODE/ComGraphic/project-space/src/shader.fs");
        text_renderer = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);

        initBuffer();

        std::cout << "Graphic Engine start running...\n";
    }

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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanceVBO);
        glGenBuffers(1, &colorVBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLists.size() * sizeof(unsigned int), indexLists.data(), GL_STATIC_DRAW);

        // define data format stored in vertex buffers
        // [attribute No.0 | 3 elements (x,y,z) | float type | no normalized | total size | offset 0]
        // this one for vertex xyz
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); // location = 0

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //  config instanceVBO
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // position instance buffer for each cube
        // we will change each object position through this "modelInstance" array
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelInstance.size(), modelInstance.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1); // location = 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glVertexAttribDivisor(1, 1);

        // config colorVBO
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colorInstance.size(), colorInstance.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2); // location = 2
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);
        glVertexAttribDivisor(2, 1);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }

    void engineUpdate(void (*user_callback)(float dt))
    {
        // compute delta time
        float current_time = static_cast<float>(glfwGetTime());
        deltaTime = current_time - prevTime;
        prevTime = current_time;

        // input detect
        processInput();
        processMouse();

        user_callback(deltaTime);

        shader->use();

        // custom callback

        glBindVertexArray(VAO);

        // camera view update
        shader->setMat4("view", camera.GetViewMatrix());

        // update position buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, modelInstance.size() * sizeof(glm::vec3), modelInstance.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
        // update color buffer
        // glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, colorInstance.size() * sizeof(glm::vec4), colorInstance.data());
        // glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
        glDrawElementsInstanced(GL_TRIANGLES, indexLists.size(), GL_UNSIGNED_INT, 0, modelInstance.size()); // draw instances

        // text must draw last in order to blend with environment
        glm::vec3 cam_pos = this->camera.Position;
        this->text_renderer->renderText(
            "Pos (" + std::to_string(cam_pos.x) + "," + std::to_string(cam_pos.y) + "," + std::to_string(cam_pos.x) + ")",
            25, 100, 1.0f,
            glm::vec3(0.0f, 1.0f, 0.0f)

        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void updateVertexVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_STATIC_DRAW);
    }
    void updateIndexEBO()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLists.size() * sizeof(unsigned int), indexLists.data(), GL_STATIC_DRAW);
    }
    void updateInstanceVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelInstance.size(), modelInstance.data(), GL_DYNAMIC_DRAW);
    }
    void updateColorVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colorInstance.size(), colorInstance.data(), GL_DYNAMIC_DRAW);
    }

    std::vector<glm::vec3> *getInstanceBuffer()
    {
        return &this->modelInstance;
    }
    std::vector<glm::vec4> *getColorBuffer()
    {
        return &this->colorInstance;
    }
    std::vector<float> &getVertexBuffer()
    {
        return this->vertexLists;
    }
    std::vector<unsigned int> &getIndexBuffer()
    {
        return this->indexLists;
    }

public:
    TextRenderer *text_renderer;
    Camera camera;

private:
    const int SCR_WIDTH = 1080;
    const int SCR_HEIGHT = 720;

    GLFWwindow *window = nullptr;
    Shader *shader;

    unsigned int VBO, VAO, instanceVBO, colorVBO, EBO; // bufferid
    float deltaTime, prevTime;                         // for frame independent rendering

    BaseObject *cube;

    //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
    // these will be binded as a buffer
    std::vector<float> vertexLists;
    std::vector<unsigned int> indexLists;
    std::vector<glm::vec3> modelInstance;
    std::vector<glm::vec4> colorInstance;
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
