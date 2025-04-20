#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include <Graphic/shader_s.h>
#include <Graphic/TextRenderer.h>
#include <Graphic/Camera.h>
#include <Graphic/GUIManager.h>
#include <Physics/PhysicsObject.h>
#include <Physics/PhysicsEngine.h>

class GraphicEngine
{
public:
    GraphicEngine() : cursor_hidden(true)
    {

        // init class member
        camera = Camera(glm::vec3(0.0f, 20.0f, -100.0f));

        vertexLists = std::vector<float>();
        indexLists = std::vector<unsigned int>();
        modelInstance = std::vector<glm::vec3>();
        container_vertices = std::vector<float>();
        container_indices = std::vector<unsigned int>();

        // projection matrix
        projection = glm::perspective(glm::radians(53.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, RENDER_DISTANCE);

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
        shader = new Shader("D:/CODE/ComGraphic/project-rework/src/shader.vs", "D:/CODE/ComGraphic/project-rework/src/shader.fs");
        container_shader = new Shader("D:/CODE/ComGraphic/project-rework/src/container_shader.vs", "D:/CODE/ComGraphic/project-rework/src/container_shader.fs");
        text_renderer = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
        // text_renderer->loadFont("D:/CODE/ComGraphic/project-space/resources/fonts/OpenSans-Regular.ttf", 24);

        gui_mgr = new GUIManager(this->window); // init GUI window

        initBuffer();

        std::cout << "Graphic Engine start running...\n";
    }

    // initialize program window
    int initWindow(int width, int height, const char *title)
    {
        glfwSetErrorCallback(glfw_error_callback);
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        window = glfwCreateWindow(width, height, "Fluid Particle Simulation", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetWindowUserPointer(window, this); // Store 'this' inside GLFW window
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

        // z-axis drawing order
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //==================================================================================================
        //==================================================================================================

        // glGenVertexArrays(1, &VAO);
        // glGenBuffers(1, &VBO);
        // glGenBuffers(1, &EBO);
        // glGenBuffers(1, &instanceVBO);
        // glGenBuffers(1, &colorVBO);

        // // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        // glBindVertexArray(VAO);

        // glBindBuffer(GL_ARRAY_BUFFER, VBO); // begin config VBO
        // glBufferData(GL_ARRAY_BUFFER, vertexLists.size() * sizeof(float), vertexLists.data(), GL_STATIC_DRAW);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // begin config EBO
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexLists.size() * sizeof(unsigned int), indexLists.data(), GL_STATIC_DRAW);

        // // Position attribute
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        // glEnableVertexAttribArray(0); // (location = 0)

        // //  config instanceVBO
        // // position (x,y,z) for each object
        // // we will change each object position through this "modelInstance" array
        // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelInstance.size(), modelInstance.data(), GL_DYNAMIC_DRAW);
        // glEnableVertexAttribArray(1); // (location = 1)
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        // glVertexAttribDivisor(1, 1);

        // // color attribute
        // glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colorInstance.size(), colorInstance.data(), GL_DYNAMIC_DRAW);
        // glEnableVertexAttribArray(2); // (location = 2)
        // glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);
        // glVertexAttribDivisor(2, 1);

        //==================================================================================================
        //==================================================================================================

        // Buffer config for particle container
        glGenVertexArrays(1, &containerVAO);
        glGenBuffers(1, &containerVBO);
        glGenBuffers(1, &containerEBO);

        glBindVertexArray(containerVAO);

        glBindBuffer(GL_ARRAY_BUFFER, containerVBO);
        glBufferData(GL_ARRAY_BUFFER, container_vertices.size() * sizeof(float), container_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, containerEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, container_indices.size() * sizeof(unsigned int), container_indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers for box
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); // (location = 0)
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); // (location = 1)

        // Unbind VAO
        glBindVertexArray(0);
    }

    /**
     * ## will be called every render loop
     * user can provide additional callback function in main.cpp
     */
    void engineUpdate(void (*user_callback)(float dt))
    {
        // compute delta time
        float current_time = static_cast<float>(glfwGetTime());
        deltaTime = current_time - prevTime;
        prevTime = current_time;

        // input detect
        processInput();
        processMouse();

        user_callback(deltaTime); // custom callback

        // //====================[general mesh rendering]====================
        // // update position buffer
        // glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, modelInstance.size() * sizeof(glm::vec3), modelInstance.data());
        // glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
        // // update color buffer
        // glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, colorInstance.size() * sizeof(glm::vec4), colorInstance.data());
        // glBindBuffer(GL_ARRAY_BUFFER, 0);                                                                   // Unbind
        // glDrawElementsInstanced(GL_TRIANGLES, indexLists.size(), GL_UNSIGNED_INT, 0, modelInstance.size()); // draw instances
        // //============================================================

        renderContainer(true);
        renderPhysicsParticles(); // render particles
        gui_mgr->showGUI();       // render GUI

        // text should draw last in order to blend with environment
        glm::vec3 cam_pos = this->camera.Position;
        this->text_renderer->renderText(
            "Pos (" + std::to_string(cam_pos.x) + "," + std::to_string(cam_pos.y) + "," + std::to_string(cam_pos.z) + ")",
            10, 100, 0.5f,
            glm::vec3(0.0f, 1.0f, 0.0f));
        this->text_renderer->renderText(
            "FPS: " + std::to_string(1.0f / deltaTime),
            10, 50, 0.5f,
            glm::vec3(0.0f, 1.0f, 0.0f));

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

    void updateContainerBuffer()
    {
        glBindBuffer(GL_ARRAY_BUFFER, containerVBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, container_vertices.size() * sizeof(float), container_vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, containerEBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, container_indices.size() * sizeof(unsigned int), container_indices.data(), GL_STATIC_DRAW);
    }

    void setContainer(float width, float height, float depth, glm::vec3 color)
    {
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;
        float halfDepth = depth / 2.0f;

        // Vertex data format: x, y, z, nx, ny, nz
        // Front face
        this->container_vertices = {
            // Front face
            -halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,
            halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,
            -halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,

            // Back face
            -halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,
            -halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,

            // Top face
            -halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b,
            -halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b,

            // Bottom face
            -halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,
            -halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,

            // Right face
            halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b,
            halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,
            halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,

            // Left face
            -halfWidth, -halfHeight, -halfDepth, color.r, color.g, color.b,
            -halfWidth, -halfHeight, halfDepth, color.r, color.g, color.b,
            -halfWidth, halfHeight, halfDepth, color.r, color.g, color.b,
            -halfWidth, halfHeight, -halfDepth, color.r, color.g, color.b};

        // Indices for 6 faces (2 triangles per face)
        this->container_indices = {
            0, 1, 2, 2, 3, 0,       // Front face
            4, 5, 6, 6, 7, 4,       // Back face
            8, 9, 10, 10, 11, 8,    // Top face
            12, 13, 14, 14, 15, 12, // Bottom face
            16, 17, 18, 18, 19, 16, // Right face
            20, 21, 22, 22, 23, 20  // Left face
        };

        this->updateContainerBuffer();
    }

    void renderContainer(bool is_wireframe)
    {
        container_shader->use(); // swtich to container shader
        container_shader->setMat4("view", camera.GetViewMatrix());
        container_shader->setMat4("projection", projection);
        container_shader->setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(containerVAO);

        if (is_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // enter wireframe mode
        }
        glDrawElements(GL_TRIANGLES, container_indices.size(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // exit wireframe mode
    }

    //======================[PhysicsEngine-related method]==========================

    // in order to render physics particle from solver
    void bindWithPhysicsEngine(PhysicsEngine *physics_engine)
    {
        // bind physics engine to this class member
        this->physics_data.p = physics_engine;

        // generate all VAO/VBO pointers
        glGenVertexArrays(1, &physics_data.particlesVAO);
        glGenBuffers(1, &physics_data.verticesVBO);
        glGenBuffers(1, &physics_data.indicesEBO);
        glGenBuffers(1, &physics_data.positionsVBO);
        glGenBuffers(1, &physics_data.colorsVBO);

        // start config particles VAO/VBO
        glBindVertexArray(physics_data.particlesVAO);

        // bind sphere mesh data
        glBindBuffer(GL_ARRAY_BUFFER, physics_data.verticesVBO); // begin config VBO
        glBufferData(GL_ARRAY_BUFFER, SPHSolver::sphereVertices.size() * sizeof(float), SPHSolver::sphereVertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, physics_data.indicesEBO); // begin config EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPHSolver::sphereIndices.size() * sizeof(unsigned int), SPHSolver::sphereIndices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0); // (location = 0)

        //  config positionsVBO
        // position (x,y,z) for each object
        // we will change each object position through this one
        glBindBuffer(GL_ARRAY_BUFFER, physics_data.positionsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * physics_data.p->sph_solver->positions.size(), physics_data.p->sph_solver->positions.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1); // (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glVertexAttribDivisor(1, 1);

        // color attribute
        // we will change each object color through this one
        glBindBuffer(GL_ARRAY_BUFFER, physics_data.colorsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * physics_data.p->sph_solver->colors.size(), physics_data.p->sph_solver->colors.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2); // (location = 2)
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);
        glVertexAttribDivisor(2, 1);

        gui_mgr->solver = physics_data.p->sph_solver; // connect solver with GUI
    }

    void renderPhysicsParticles()
    {
        // if physices engine available
        if (physics_data.p != nullptr && physics_data.p->sph_solver != nullptr)
        {
            shader->use();                                   // swtich to normal shader
            shader->setMat4("view", camera.GetViewMatrix()); // camera view update
            shader->setMat4("projection", projection);

            // start using sphere mesh
            glBindVertexArray(physics_data.particlesVAO);

            // update position buffer
            glBindBuffer(GL_ARRAY_BUFFER, physics_data.positionsVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, physics_data.p->sph_solver->positions.size() * sizeof(glm::vec3), physics_data.p->sph_solver->positions.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind
            // update color buffer
            glBindBuffer(GL_ARRAY_BUFFER, physics_data.colorsVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, physics_data.p->sph_solver->colors.size() * sizeof(glm::vec4), physics_data.p->sph_solver->colors.data());
            glBindBuffer(GL_ARRAY_BUFFER, 0);                                                                                                                   // Unbind
            glDrawElementsInstanced(GL_TRIANGLES, SPHSolver::sphereIndices.size(), GL_UNSIGNED_INT, 0, physics_data.p->sph_solver->positions.size()); // draw instances
        }
    }

    // reallocate buffer of particles data
    void updateSolverBuffer()
    {
        if (physics_data.p != nullptr && physics_data.p->sph_solver != nullptr)
            glBindBuffer(GL_ARRAY_BUFFER, physics_data.positionsVBO);
        glBufferData(GL_ARRAY_BUFFER, physics_data.p->sph_solver->positions.size() * sizeof(glm::vec3), physics_data.p->sph_solver->positions.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, physics_data.colorsVBO);
        glBufferData(GL_ARRAY_BUFFER, physics_data.p->sph_solver->colors.size() * sizeof(glm::vec3), physics_data.p->sph_solver->colors.data(), GL_DYNAMIC_DRAW);
    }
    //===============================================================================

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
    struct PhysicsEngineData
    {
        PhysicsEngine *p = nullptr;
        unsigned int particlesVAO,
            verticesVBO, indicesEBO, positionsVBO, colorsVBO;
    };
    PhysicsEngineData physics_data;

    TextRenderer *text_renderer;
    GLFWwindow *window = nullptr;
    Camera camera;

    GUIManager *gui_mgr;

    bool cursor_hidden;

    const int SCR_WIDTH = 1080;
    const int SCR_HEIGHT = 720;
    const float RENDER_DISTANCE = 1000.0f;

    Shader *shader;
    Shader *container_shader;

    float deltaTime, prevTime; // for frame independent rendering
    // Particle

    unsigned int VBO, VAO, instanceVBO, colorVBO, EBO; // bufferid
    // these will be binded as a buffer
    std::vector<float> vertexLists;
    std::vector<unsigned int> indexLists;
    std::vector<glm::vec3> modelInstance;
    std::vector<glm::vec4> colorInstance;

    // Container for particle
    unsigned int containerVAO, containerVBO, containerEBO;
    std::vector<float> container_vertices;
    std::vector<unsigned int> container_indices;

    // camera
    bool firstMouse;
    float lastX;
    float lastY;

    // transformation matrix for vertex shader
    glm::mat4 view;
    glm::mat4 projection;

private:
    //======================[PRIVATE METHOD]========================
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

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

    void processMouse()
    {
        // fixed camera when show cursor
        if (!cursor_hidden)
        {
            return;
        }

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

    static void glfw_error_callback(int error, const char *description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }
};
