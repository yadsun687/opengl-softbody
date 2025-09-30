#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem.h>
#include <Graphic/shdaer_m.h>
#include <camera.h>
#include <objectloader.h>

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

struct SoftBodyObject
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
};
struct MeshData
{
    unsigned int index;
    glm::vec2 texCoord;
};
struct RenderAttribute
{
    glm::vec3 vertex;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadTexture(char const *path);
void renderSoftBody();
void stepSolver(float frameTime);
void stepSolver(float frameTime, SoftBodyMesh &softBody);
void stepSolver(float frameTime, std::vector<SoftBodyObject *> &softBodies);
std::vector<RenderAttribute> softBodyToVertex(std::vector<SoftBodyObject *> &objs);
std::vector<RenderAttribute> softBodyToVertex(SoftBodyMesh &objs);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

std::vector<glm::vec3> softBodyVertices = {
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, -0.5f, -0.5f),
    glm::vec3(0.5f, 0.5f, -0.5f),
    glm::vec3(-0.5f, 0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f, 0.5f),
    glm::vec3(0.5f, -0.5f, 0.5f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(-0.5f, 0.5f, 0.5f)};
std::vector<unsigned int> softBodyIndices = {
    // back face
    0, 1, 2,
    2, 3, 0,
    // front face
    4, 5, 6,
    6, 7, 4,
    // left face
    0, 4, 7,
    7, 3, 0,
    // right face
    1, 5, 6,
    6, 2, 1,
    // top face
    3, 2, 6,
    6, 7, 3,
    // bottom face;
    0, 1, 5,
    5, 4, 0};

std::vector<MeshData> cubeMeshData = {
    // index of vertex        // texture coords
    {0, {0.0f, 0.0f}},
    {1, {1.0f, 0.0f}},
    {2, {1.0f, 1.0f}},
    {2, {1.0f, 1.0f}},
    {3, {0.0f, 1.0f}},
    {0, {0.0f, 0.0f}},

    {4, {0.0f, 0.0f}},
    {5, {1.0f, 0.0f}},
    {6, {1.0f, 1.0f}},
    {6, {1.0f, 1.0f}},
    {7, {0.0f, 1.0f}},
    {4, {0.0f, 0.0f}},

    {0, {0.0f, 1.0f}},
    {4, {0.0f, 0.0f}},
    {7, {1.0f, 0.0f}},
    {7, {1.0f, 0.0f}},
    {3, {1.0f, 1.0f}},
    {0, {0.0f, 1.0f}},

    {1, {0.0f, 1.0f}},
    {5, {0.0f, 0.0f}},
    {6, {1.0f, 0.0f}},
    {6, {1.0f, 0.0f}},
    {2, {1.0f, 1.0f}},
    {1, {0.0f, 1.0f}},

    {3, {0.0f, 1.0f}},
    {2, {1.0f, 1.0f}},
    {6, {1.0f, 0.0f}},
    {6, {1.0f, 0.0f}},
    {7, {0.0f, 0.0f}},
    {3, {0.0f, 1.0f}},

    {0, {0.0f, 1.0f}},
    {1, {1.0f, 1.0f}},
    {5, {1.0f, 0.0f}},
    {5, {1.0f, 0.0f}},
    {4, {0.0f, 0.0f}},
    {0, {0.0f, 1.0f}}};
std::vector<glm::vec3> positions(softBodyVertices);
std::vector<glm::vec3> velocities(positions.size(), glm::vec3(0.0f, 0.0f, 0.0f));

bool isSimulationPaused = true;
glm::vec3 userForce(0.0f);

const float GRAVITY = 0.0f;
const float SPRING_CONSTANT = 1.0f;
const float SPRING_DAMPING = 0.9f;
const float SHAPE_STIFFNESS = 0.00005f;

int main()
{
    // ObjectLoader
    SoftBodyMesh sdbmesh;
    ObjectLoader::loadOBJ("D:/CODE/ComGraphic/project-rework/resources/objects/rabbit_with_texture.obj", sdbmesh);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("D:/CODE/ComGraphic/project-rework/src/multiple_lights.vs", "D:/CODE/ComGraphic/project-rework/src/multiple_lights.fs");
    Shader lightCubeShader("D:/CODE/ComGraphic/project-rework/src/light_cube.vs", "D:/CODE/ComGraphic/project-rework/src/light_cube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float basicCubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // // first, configure the cube's VAO (and VBO)
    // unsigned int VBO, softBodyVAO;
    // glGenVertexArrays(1, &softBodyVAO);
    // glGenBuffers(1, &VBO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // glBindVertexArray(softBodyVAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO, lightCubeVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basicCubeVertices), &basicCubeVertices, GL_STATIC_DRAW);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int softBodyVAO, softBodyVBO, softBodyEBO;
    glGenVertexArrays(1, &softBodyVAO);
    glGenBuffers(1, &softBodyVBO);
    glGenBuffers(1, &softBodyEBO);
    glBindVertexArray(softBodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, softBodyVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, softBodyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, softBodyIndices.size() * sizeof(unsigned int), &softBodyIndices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // create new softbody
    SoftBodyObject cube1 = {
        std::vector<glm::vec3>(softBodyVertices),
        std::vector<glm::vec3>(softBodyVertices.size(), glm::vec3(0.0f))};

    SoftBodyObject cube2 = {
        std::vector<glm::vec3>(softBodyVertices),
        std::vector<glm::vec3>(softBodyVertices.size(), glm::vec3(0.0f))};

    glm::mat4 t1 = glm::mat4(1.0f);
    t1 = glm::translate(t1, glm::vec3(2.0f, 1.0f, 0.0f));
    for (auto &pos : cube2.positions)
    {
        pos = glm::vec3(t1 * glm::vec4(pos, 1.0f));
    }
    std::vector<SoftBodyObject *> cubeList = {&cube1, &cube2};

    // std::vector<RenderAttribute> cubeListRender = softBodyToVertex(cubeList);

    // for (auto &vel : sdbmesh.velocities)
    // {
    //     vel.x = 0.4f;
    //     vel.y = 0.3f;
    //     vel.z = -0.2f;
    // }
    std::vector<RenderAttribute> testMesh = softBodyToVertex(sdbmesh);

    unsigned int newSoftBodyVAO, newSoftBodyVBO;
    glGenVertexArrays(1, &newSoftBodyVAO);
    glGenBuffers(1, &newSoftBodyVBO);
    glBindVertexArray(newSoftBodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, newSoftBodyVBO);

    glBufferData(GL_ARRAY_BUFFER, testMesh.size() * sizeof(RenderAttribute), testMesh.data(), GL_DYNAMIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, vertex.vertices.size() * sizeof(glm::vec3) + vertex.texCord.size() * sizeof(glm::vec3), cubeListRender.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderAttribute), (void *)offsetof(RenderAttribute, vertex));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderAttribute), (void *)offsetof(RenderAttribute, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderAttribute), (void *)offsetof(RenderAttribute, texCoord));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // --- set up a simple white plane (floor) ---
    // We'll use a large quad made from two triangles. The plane only needs positions (location = 0)
    unsigned int planeVAO, planeVBO;
    float planeVertices[] = {
        // first triangle
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
        2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
        // second triangle
        2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
        -2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // unbind for cleanliness
    glBindVertexArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------

    unsigned int specularMap = loadTexture("D:/CODE/ComGraphic/project-rework/resources/textures/marble_specular.png");
    unsigned int diffuseMap = loadTexture("D:/CODE/ComGraphic/project-rework/resources/textures/marble.png");
    unsigned int stoneTexture = loadTexture("D:/CODE/ComGraphic/project-rework/resources/textures/stone.png");
    unsigned int woodTexture = loadTexture("D:/CODE/ComGraphic/project-rework/resources/textures/wood_floor.png");
    unsigned int woodTextureSpecular = loadTexture("D:/CODE/ComGraphic/project-rework/resources/textures/wood_floor_specular.png");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    glm::mat4 cubeTransform = glm::mat4(1.0f);
    cubeTransform = glm::rotate(cubeTransform, glm::radians(-20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    for (auto &pos : cube1.positions)
    {
        pos = glm::vec3(cubeTransform * glm::vec4(pos, 1.0f));
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // [Draw the white plane (floor) with stone texture]
        glm::mat4 planeModel = glm::mat4(1.0f);
        lightingShader.setMat4("model", planeModel);
        glBindVertexArray(planeVAO);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, woodTextureSpecular);
        // draw 2 triangles -> 6 vertices
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // draw the cubes
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glBindVertexArray(softBodyVAO);
        // glBindBuffer(GL_ARRAY_BUFFER, softBodyVBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
        // glDrawElements(GL_TRIANGLES, softBodyIndices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(newSoftBodyVAO);
        glBindBuffer(GL_ARRAY_BUFFER, newSoftBodyVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, testMesh.size() * sizeof(RenderAttribute), testMesh.data());
        // glDrawElements(GL_TRIANGLES, softBodyIndices.size(), GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, testMesh.size());
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
        for (unsigned int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // stepSolver(1.0f / 720.0f);
        if (!isSimulationPaused)
        {
            for (auto &vel : sdbmesh.velocities)
            {

                vel += userForce;
            }
            userForce = glm::vec3(0.0f);
            stepSolver(1.0f / 720.0f, sdbmesh);
            testMesh = softBodyToVertex(sdbmesh);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &softBodyVAO);
    // glDeleteVertexArrays(1, &lightCubeVAO);
    // glDeleteVertexArrays(1, &planeVAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &planeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_P:
            isSimulationPaused = !isSimulationPaused;
            break;
        case GLFW_KEY_F:
            userForce = camera.Front * 0.5f;
            break;
        }
    }
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void renderSoftBody()
{
    // Placeholder for soft body rendering logic
}

void stepSolver(float frameTime)
{
    for (size_t i = 0; i < softBodyVertices.size(); ++i)
    {
        for (size_t j = i + 1; j < softBodyVertices.size(); ++j)
        {
            // constraint only between connected vertices
            if (i == j)
                continue;
            float constraint_dist = glm::distance(softBodyVertices[i], softBodyVertices[j]); // sprint rest length
            if (constraint_dist < 1.5f)
            {
                glm::vec3 dir = glm::normalize(positions[j] - positions[i]);

                float spring_force = SPRING_CONSTANT * (glm::distance(positions[j], positions[i]) - constraint_dist);
                float damping_force = SPRING_DAMPING * glm::dot(dir, velocities[j] - velocities[i]);

                glm::vec3 force = dir * (spring_force + damping_force);

                velocities[i] += force * frameTime;
                velocities[j] -= force * frameTime;
            }
        }
    }

    float damping = 0.98f;
    for (size_t i = 0; i < positions.size(); ++i)
    {
        positions[i] += velocities[i] * frameTime;
        positions[i].y -= GRAVITY * frameTime; // gravity

        // collision with ground plane at y = -1.0f
        if (positions[i].y < -1.0f)
        {
            positions[i].y = -1.0f;
            velocities[i].y *= -1.0f * damping; // Simple collision response
        }
    }
}

void stepSolver(float frameTime, std::vector<SoftBodyObject *> &softBodies)
{
    for (auto &sbd : softBodies)
    {

        for (size_t i = 0; i < softBodyVertices.size(); ++i)
        {
            for (size_t j = i + 1; j < softBodyVertices.size(); ++j)
            {
                // constraint only between connected vertices
                if (i == j)
                    continue;
                float constraint_dist = glm::distance(softBodyVertices[i], softBodyVertices[j]); // sprint rest length
                if (constraint_dist < 1.5f)
                {
                    glm::vec3 dir = glm::normalize(sbd->positions[j] - sbd->positions[i]);

                    float spring_force = SPRING_CONSTANT * (glm::distance(sbd->positions[j], sbd->positions[i]) - constraint_dist);
                    float damping_force = SPRING_DAMPING * glm::dot(dir, sbd->velocities[j] - sbd->velocities[i]);

                    glm::vec3 force = dir * (spring_force + damping_force);

                    sbd->velocities[i] += force * frameTime;
                    sbd->velocities[j] -= force * frameTime;
                }
            }
        }

        float damping = 1.0f;
        for (size_t i = 0; i < sbd->positions.size(); ++i)
        {
            sbd->positions[i] += sbd->velocities[i] * frameTime;
            sbd->positions[i].y -= GRAVITY * frameTime; // gravity

            // collision with ground plane at y = -1.0f
            if (sbd->positions[i].y < -1.0f)
            {
                sbd->positions[i].y = -1.0f;
                sbd->velocities[i].y *= -1.0f * damping; // Simple collision response
            }
        }
    }
}

void stepSolver(float frameTime, SoftBodyMesh &softBody)
{

    glm::vec3 x_cm(0.0f);
    for (size_t i = 0; i < softBody.vertices.size(); ++i)
    {
        softBody.velocities[i].y -= GRAVITY * frameTime;
        x_cm += softBody.vertices[i];
    }
    x_cm /= softBody.vertices.size();

    glm::mat3 A_pq(0.0f);
    for (size_t i = 0; i < softBody.vertices.size(); ++i)
    {
        glm::vec3 p_i = softBody.vertices[i] - x_cm; // Current position relative to center
        glm::vec3 q_i = softBody.x_offset_zero[i];   // Rest position relative to center
        A_pq += glm::outerProduct(p_i, q_i);
    }
    Eigen::Matrix3d apq;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            apq(i, j) = A_pq[j][i]; // Note: GLM uses column-major, Eigen defaults to column-major but can be row-major
        }
    }

    Eigen::JacobiSVD<Eigen::Matrix3d> svd(apq, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix3d R_eigen = svd.matrixU() * svd.matrixV().transpose();

    // Ensure the rotation matrix is proper (no reflection/inversion)
    if (R_eigen.determinant() < 0)
    {
        Eigen::Matrix3d U_mod = svd.matrixU();
        U_mod.col(2) *= -1; // Flip the last column
        R_eigen = U_mod * svd.matrixV().transpose();
    }

    // std::cout << R_eigen;
    // Convert Eigen rotation matrix back to GLM
    glm::mat3 R;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            R[j][i] = R_eigen(i, j);
        }
    }

    for (size_t i = 0; i < softBody.vertices.size(); ++i)
    {
        glm::vec3 goal_pos = (R * softBody.x_offset_zero[i]) + x_cm;
        glm::vec3 goal_pos_diff = (goal_pos - softBody.vertices[i]) * (SHAPE_STIFFNESS / frameTime);

        softBody.velocities[i] += goal_pos_diff;
        // std::cout << goal_pos_diff.x << "," << goal_pos_diff.y << "," << goal_pos_diff.z << "\n";
    }

    for (auto &constraint : softBody.structuralPairs)
    {
        glm::vec3 i = softBody.vertices[constraint.pair.first];
        glm::vec3 j = softBody.vertices[constraint.pair.second];
        glm::vec3 vi = softBody.velocities[constraint.pair.first];
        glm::vec3 vj = softBody.velocities[constraint.pair.second];

        float current_dist = glm::length(i - j);
        if (current_dist < 1e-6f)
            continue;

        glm::vec3 dir = glm::normalize(j - i);

        float spring_force = SPRING_CONSTANT * (current_dist - constraint.distance);
        float damping_force = SPRING_DAMPING * glm::dot(dir, vj - vi);

        glm::vec3 force = dir * (spring_force + damping_force) * frameTime;

        softBody.velocities[constraint.pair.first] += force;
        softBody.velocities[constraint.pair.second] -= force;
    }

    float damping = 1.0f;
    for (size_t i = 0; i < softBody.vertices.size(); ++i)
    {

        softBody.vertices[i] += softBody.velocities[i] * frameTime;
        // collision with ground plane at y = -1.0f
        if (softBody.vertices[i].y < -2.0f)
        {
            softBody.vertices[i].y = -2.0f;
            softBody.velocities[i].y *= -1.0f * damping; // Simple collision response
        }
        if (softBody.vertices[i].y > 2.0f)
        {
            softBody.vertices[i].y = 2.0f;
            softBody.velocities[i].y *= -1.0f * damping; // Simple collision response
        }
        if (softBody.vertices[i].x > 2.0f)
        {
            softBody.vertices[i].x = 2.0f;
            softBody.velocities[i].x *= -1.0f * damping; // Simple collision response
        }
        if (softBody.vertices[i].x < -2.0f)
        {
            softBody.vertices[i].x = -2.0f;
            softBody.velocities[i].x *= -1.0f * damping; // Simple collision response
        }
        if (softBody.vertices[i].z > 2.0f)
        {
            softBody.vertices[i].z = 2.0f;
            softBody.velocities[i].z *= -1.0f * damping; // Simple collision response
        }
        if (softBody.vertices[i].z < -2.0f)
        {
            softBody.vertices[i].z = -2.0f;
            softBody.velocities[i].z *= -1.0f * damping; // Simple collision response
        }
    }
}

std::vector<RenderAttribute> softBodyToVertex(std::vector<SoftBodyObject *> &objs)
{
    // number of vertex equal to mesh indices size
    std::vector<RenderAttribute> result;
    result.reserve(softBodyIndices.size() * objs.size());

    for (auto &obj : objs)
    {
        for (MeshData vert : cubeMeshData)
        {
            result.push_back({obj->positions[vert.index], {0.0f, 0.0f, 0.0f}, vert.texCoord});
        }
    }

    return result;
}

std::vector<RenderAttribute> softBodyToVertex(SoftBodyMesh &objs)
{

    // number of vertex equal to mesh indices size
    std::vector<RenderAttribute> result;
    size_t total_face = objs.faces.size() / 3; // vertex & texture & normal = 3
    result.reserve(total_face);

    for (size_t face_idx = 0; face_idx < objs.faces.size(); face_idx += 3)
    {
        result.push_back({objs.vertices[objs.faces[face_idx]], objs.normals[objs.faces[face_idx + 2]], objs.texCoords[objs.faces[face_idx + 1]]});
    }

    return result;
}