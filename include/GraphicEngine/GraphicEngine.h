#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <shader/shader_s.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

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

        shader = new Shader("src/shader.vs", "src/shader.fs");
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

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanceVBO); 

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLists.size() * sizeof(unsigned int), indexLists.data(), GL_DYNAMIC_DRAW);

        // define data format stored in vertex buffers
        // [attribute No.0 | 3 elements (x,y,z) | float type | no normalized | total size | offset 0]
        // first 3 for vertex xyz, next 4 for color rgba
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
        // begin config instanceVBO
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // position instance buffer for each cube
        // we will change each object position through this "modelInstance" array
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelInstance.size(), modelInstance.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
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

        shader->use();

        // custom callback
        user_callback(deltaTime);

        glBindVertexArray(VAO);

        // camera view update
        shader->setMat4("view", camera.GetViewMatrix());

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER , 0 , modelInstance.size() * sizeof(glm::vec3) , modelInstance.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
        glDrawElementsInstanced(GL_TRIANGLES, indexLists.size(), GL_UNSIGNED_INT, 0, modelInstance.size()); // draw instances

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void updateVertexVBO(){
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_DYNAMIC_DRAW);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void updateIndexEBO(){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLists.size() * sizeof(unsigned int), indexLists.data(), GL_DYNAMIC_DRAW);
    }
    void updateinstanceVBO(){
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // transformation matrix buffer for each cube
        // we will change each object position through this "modelInstance" array
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelInstance.size(), modelInstance.data(), GL_DYNAMIC_DRAW);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    std::vector<glm::vec3>* getInstanceBuffer(){
        return &modelInstance;
    }
    std::vector<float>& getVertexBuffer(){
        return vertexLists;
    }
    std::vector<unsigned int>& getIndexBuffer(){
        return indexLists;
    }

private:
    const int SCR_WIDTH = 1080;
    const int SCR_HEIGHT = 720;

    GLFWwindow *window = nullptr;
    Shader *shader;
    unsigned int VBO, VAO, instanceVBO, EBO; // buffer
    float deltaTime, prevTime;               // for frame independent rendering

    Camera camera;


    BaseObject *cube;

    //[TEST][TEST][TEST][TEST][TEST][TEST][TEST][TEST]
    std::vector<BaseObject> cubeLists;
    //these will be binded as a buffer
    std::vector<float> vertexLists;
    std::vector<unsigned int> indexLists;
    std::vector<glm::vec3> modelInstance;
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
