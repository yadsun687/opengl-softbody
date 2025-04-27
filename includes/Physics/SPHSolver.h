#pragma once

#include <math.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <omp.h>
#include <algorithm>
#include <random>

class SPHSolver
{
public:
    // particle mesh (sphere)
    inline static const std::vector<float> sphereVertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f};
    // inline static const std::vector<float> sphereVertices = {
    //     -0.00000004, 1.00000000, 0.00000000,
    //     -0.00000004, 1.00000000, 0.00000002,
    //     -0.00000003, 1.00000000, 0.00000003,
    //     -0.00000002, 1.00000000, 0.00000004,
    //     0.00000000, 1.00000000, 0.00000004,
    //     0.00000002, 1.00000000, 0.00000004,
    //     0.00000003, 1.00000000, 0.00000003,
    //     0.00000004, 1.00000000, 0.00000002,
    //     0.00000004, 1.00000000, -0.00000000,
    //     0.00000004, 1.00000000, -0.00000002,
    //     0.00000003, 1.00000000, -0.00000003,
    //     0.00000002, 1.00000000, -0.00000004,
    //     -0.00000000, 1.00000000, -0.00000004,
    //     -0.00000002, 1.00000000, -0.00000004,
    //     -0.00000003, 1.00000000, -0.00000003,
    //     -0.00000004, 1.00000000, -0.00000002,
    //     -0.00000004, 1.00000000, 0.00000000,
    //     0.38268343, 0.92387950, 0.00000000,
    //     0.35355338, 0.92387950, -0.14644662,
    //     0.27059805, 0.92387950, -0.27059805,
    //     0.14644660, 0.92387950, -0.35355338,
    //     -0.00000002, 0.92387950, -0.38268343,
    //     -0.14644665, 0.92387950, -0.35355338,
    //     -0.27059805, 0.92387950, -0.27059805,
    //     -0.35355341, 0.92387950, -0.14644654,
    //     -0.38268343, 0.92387950, 0.00000003,
    //     -0.35355338, 0.92387950, 0.14644660,
    //     -0.27059799, 0.92387950, 0.27059808,
    //     -0.14644650, 0.92387950, 0.35355344,
    //     0.00000000, 0.92387950, 0.38268343,
    //     0.14644668, 0.92387950, 0.35355335,
    //     0.27059814, 0.92387950, 0.27059796,
    //     0.35355338, 0.92387950, 0.14644660,
    //     0.38268343, 0.92387950, -0.00000007,
    //     0.70710677, 0.70710677, 0.00000000,
    //     0.65328145, 0.70710677, -0.27059805,
    //     0.49999997, 0.70710677, -0.49999997,
    //     0.27059805, 0.70710677, -0.65328145,
    //     -0.00000003, 0.70710677, -0.70710677,
    //     -0.27059811, 0.70710677, -0.65328145,
    //     -0.49999997, 0.70710677, -0.49999997,
    //     -0.65328151, 0.70710677, -0.27059793,
    //     -0.70710677, 0.70710677, 0.00000006,
    //     -0.65328145, 0.70710677, 0.27059805,
    //     -0.49999991, 0.70710677, 0.50000006,
    //     -0.27059782, 0.70710677, 0.65328157,
    //     0.00000001, 0.70710677, 0.70710677,
    //     0.27059817, 0.70710677, 0.65328139,
    //     0.50000018, 0.70710677, 0.49999982,
    //     0.65328151, 0.70710677, 0.27059805,
    //     0.70710677, 0.70710677, -0.00000012,
    //     0.92387950, 0.38268346, 0.00000000,
    //     0.85355335, 0.38268346, -0.35355341,
    //     0.65328145, 0.38268346, -0.65328145,
    //     0.35355338, 0.38268346, -0.85355335,
    //     -0.00000004, 0.38268346, -0.92387950,
    //     -0.35355344, 0.38268346, -0.85355335,
    //     -0.65328145, 0.38268346, -0.65328145,
    //     -0.85355347, 0.38268346, -0.35355324,
    //     -0.92387950, 0.38268346, 0.00000008,
    //     -0.85355335, 0.38268346, 0.35355338,
    //     -0.65328133, 0.38268346, 0.65328157,
    //     -0.35355309, 0.38268346, 0.85355347,
    //     0.00000001, 0.38268346, 0.92387950,
    //     0.35355353, 0.38268346, 0.85355330,
    //     0.65328169, 0.38268346, 0.65328121,
    //     0.85355341, 0.38268346, 0.35355338,
    //     0.92387950, 0.38268346, -0.00000016,
    //     1.00000000, 0.00000000, 0.00000000,
    //     0.92387950, 0.00000000, -0.38268346,
    //     0.70710677, 0.00000000, -0.70710677,
    //     0.38268343, 0.00000000, -0.92387950,
    //     -0.00000004, 0.00000000, -1.00000000,
    //     -0.38268352, 0.00000000, -0.92387950,
    //     -0.70710677, 0.00000000, -0.70710677,
    //     -0.92387962, 0.00000000, -0.38268328,
    //     -1.00000000, 0.00000000, 0.00000009,
    //     -0.92387950, 0.00000000, 0.38268343,
    //     -0.70710665, 0.00000000, 0.70710689,
    //     -0.38268313, 0.00000000, 0.92387968,
    //     0.00000001, 0.00000000, 1.00000000,
    //     0.38268360, 0.00000000, 0.92387944,
    //     0.70710701, 0.00000000, 0.70710653,
    //     0.92387956, 0.00000000, 0.38268343,
    //     1.00000000, 0.00000000, -0.00000017,
    //     0.92387950, -0.38268346, 0.00000000,
    //     0.85355335, -0.38268346, -0.35355341,
    //     0.65328145, -0.38268346, -0.65328145,
    //     0.35355338, -0.38268346, -0.85355335,
    //     -0.00000004, -0.38268346, -0.92387950,
    //     -0.35355344, -0.38268346, -0.85355335,
    //     -0.65328145, -0.38268346, -0.65328145,
    //     -0.85355347, -0.38268346, -0.35355324,
    //     -0.92387950, -0.38268346, 0.00000008,
    //     -0.85355335, -0.38268346, 0.35355338,
    //     -0.65328133, -0.38268346, 0.65328157,
    //     -0.35355309, -0.38268346, 0.85355347,
    //     0.00000001, -0.38268346, 0.92387950,
    //     0.35355353, -0.38268346, 0.85355330,
    //     0.65328169, -0.38268346, 0.65328121,
    //     0.85355341, -0.38268346, 0.35355338,
    //     0.92387950, -0.38268346, -0.00000016,
    //     0.70710683, -0.70710677, 0.00000000,
    //     0.65328151, -0.70710677, -0.27059808,
    //     0.50000000, -0.70710677, -0.50000000,
    //     0.27059805, -0.70710677, -0.65328151,
    //     -0.00000003, -0.70710677, -0.70710683,
    //     -0.27059811, -0.70710677, -0.65328151,
    //     -0.50000000, -0.70710677, -0.50000000,
    //     -0.65328157, -0.70710677, -0.27059796,
    //     -0.70710683, -0.70710677, 0.00000006,
    //     -0.65328151, -0.70710677, 0.27059805,
    //     -0.49999994, -0.70710677, 0.50000012,
    //     -0.27059785, -0.70710677, 0.65328163,
    //     0.00000001, -0.70710677, 0.70710683,
    //     0.27059820, -0.70710677, 0.65328145,
    //     0.50000018, -0.70710677, 0.49999985,
    //     0.65328157, -0.70710677, 0.27059805,
    //     0.70710683, -0.70710677, -0.00000012,
    //     0.38268331, -0.92387956, 0.00000000,
    //     0.35355327, -0.92387956, -0.14644657,
    //     0.27059796, -0.92387956, -0.27059796,
    //     0.14644656, -0.92387956, -0.35355327,
    //     -0.00000002, -0.92387956, -0.38268331,
    //     -0.14644660, -0.92387956, -0.35355327,
    //     -0.27059796, -0.92387956, -0.27059796,
    //     -0.35355330, -0.92387956, -0.14644650,
    //     -0.38268331, -0.92387956, 0.00000003,
    //     -0.35355327, -0.92387956, 0.14644656,
    //     -0.27059790, -0.92387956, 0.27059799,
    //     -0.14644645, -0.92387956, 0.35355332,
    //     0.00000000, -0.92387956, 0.38268331,
    //     0.14644663, -0.92387956, 0.35355324,
    //     0.27059805, -0.92387956, 0.27059788,
    //     0.35355330, -0.92387956, 0.14644656,
    //     0.38268331, -0.92387956, -0.00000007,
    //     -0.00000004, -1.00000000, 0.00000000,
    //     -0.00000004, -1.00000000, 0.00000002,
    //     -0.00000003, -1.00000000, 0.00000003,
    //     -0.00000002, -1.00000000, 0.00000004,
    //     0.00000000, -1.00000000, 0.00000004,
    //     0.00000002, -1.00000000, 0.00000004,
    //     0.00000003, -1.00000000, 0.00000003,
    //     0.00000004, -1.00000000, 0.00000002,
    //     0.00000004, -1.00000000, -0.00000000,
    //     0.00000004, -1.00000000, -0.00000002,
    //     0.00000003, -1.00000000, -0.00000003,
    //     0.00000002, -1.00000000, -0.00000004,
    //     -0.00000000, -1.00000000, -0.00000004,
    //     -0.00000002, -1.00000000, -0.00000004,
    //     -0.00000003, -1.00000000, -0.00000003,
    //     -0.00000004, -1.00000000, -0.00000002,
    //     -0.00000004, -1.00000000, 0.00000000};
    inline static const std::vector<unsigned int> sphereIndices = {
        1, 17, 18,
        2, 18, 19,
        3, 19, 20,
        4, 20, 21,
        5, 21, 22,
        6, 22, 23,
        7, 23, 24,
        8, 24, 25,
        9, 25, 26,
        10, 26, 27,
        11, 27, 28,
        12, 28, 29,
        13, 29, 30,
        14, 30, 31,
        15, 31, 32,
        16, 32, 33,
        17, 34, 18,
        18, 34, 35,
        18, 35, 19,
        19, 35, 36,
        19, 36, 20,
        20, 36, 37,
        20, 37, 21,
        21, 37, 38,
        21, 38, 22,
        22, 38, 39,
        22, 39, 23,
        23, 39, 40,
        23, 40, 24,
        24, 40, 41,
        24, 41, 25,
        25, 41, 42,
        25, 42, 26,
        26, 42, 43,
        26, 43, 27,
        27, 43, 44,
        27, 44, 28,
        28, 44, 45,
        28, 45, 29,
        29, 45, 46,
        29, 46, 30,
        30, 46, 47,
        30, 47, 31,
        31, 47, 48,
        31, 48, 32,
        32, 48, 49,
        32, 49, 33,
        33, 49, 50,
        34, 51, 35,
        35, 51, 52,
        35, 52, 36,
        36, 52, 53,
        36, 53, 37,
        37, 53, 54,
        37, 54, 38,
        38, 54, 55,
        38, 55, 39,
        39, 55, 56,
        39, 56, 40,
        40, 56, 57,
        40, 57, 41,
        41, 57, 58,
        41, 58, 42,
        42, 58, 59,
        42, 59, 43,
        43, 59, 60,
        43, 60, 44,
        44, 60, 61,
        44, 61, 45,
        45, 61, 62,
        45, 62, 46,
        46, 62, 63,
        46, 63, 47,
        47, 63, 64,
        47, 64, 48,
        48, 64, 65,
        48, 65, 49,
        49, 65, 66,
        49, 66, 50,
        50, 66, 67,
        51, 68, 52,
        52, 68, 69,
        52, 69, 53,
        53, 69, 70,
        53, 70, 54,
        54, 70, 71,
        54, 71, 55,
        55, 71, 72,
        55, 72, 56,
        56, 72, 73,
        56, 73, 57,
        57, 73, 74,
        57, 74, 58,
        58, 74, 75,
        58, 75, 59,
        59, 75, 76,
        59, 76, 60,
        60, 76, 77,
        60, 77, 61,
        61, 77, 78,
        61, 78, 62,
        62, 78, 79,
        62, 79, 63,
        63, 79, 80,
        63, 80, 64,
        64, 80, 81,
        64, 81, 65,
        65, 81, 82,
        65, 82, 66,
        66, 82, 83,
        66, 83, 67,
        67, 83, 84,
        68, 85, 69,
        69, 85, 86,
        69, 86, 70,
        70, 86, 87,
        70, 87, 71,
        71, 87, 88,
        71, 88, 72,
        72, 88, 89,
        72, 89, 73,
        73, 89, 90,
        73, 90, 74,
        74, 90, 91,
        74, 91, 75,
        75, 91, 92,
        75, 92, 76,
        76, 92, 93,
        76, 93, 77,
        77, 93, 94,
        77, 94, 78,
        78, 94, 95,
        78, 95, 79,
        79, 95, 96,
        79, 96, 80,
        80, 96, 97,
        80, 97, 81,
        81, 97, 98,
        81, 98, 82,
        82, 98, 99,
        82, 99, 83,
        83, 99, 100,
        83, 100, 84,
        84, 100, 101,
        85, 102, 86,
        86, 102, 103,
        86, 103, 87,
        87, 103, 104,
        87, 104, 88,
        88, 104, 105,
        88, 105, 89,
        89, 105, 106,
        89, 106, 90,
        90, 106, 107,
        90, 107, 91,
        91, 107, 108,
        91, 108, 92,
        92, 108, 109,
        92, 109, 93,
        93, 109, 110,
        93, 110, 94,
        94, 110, 111,
        94, 111, 95,
        95, 111, 112,
        95, 112, 96,
        96, 112, 113,
        96, 113, 97,
        97, 113, 114,
        97, 114, 98,
        98, 114, 115,
        98, 115, 99,
        99, 115, 116,
        99, 116, 100,
        100, 116, 117,
        100, 117, 101,
        101, 117, 118,
        102, 119, 103,
        103, 119, 120,
        103, 120, 104,
        104, 120, 121,
        104, 121, 105,
        105, 121, 122,
        105, 122, 106,
        106, 122, 123,
        106, 123, 107,
        107, 123, 124,
        107, 124, 108,
        108, 124, 125,
        108, 125, 109,
        109, 125, 126,
        109, 126, 110,
        110, 126, 127,
        110, 127, 111,
        111, 127, 128,
        111, 128, 112,
        112, 128, 129,
        112, 129, 113,
        113, 129, 130,
        113, 130, 114,
        114, 130, 131,
        114, 131, 115,
        115, 131, 132,
        115, 132, 116,
        116, 132, 133,
        116, 133, 117,
        117, 133, 134,
        117, 134, 118,
        118, 134, 135,
        119, 136, 120,
        120, 137, 121,
        121, 138, 122,
        122, 139, 123,
        123, 140, 124,
        124, 141, 125,
        125, 142, 126,
        126, 143, 127,
        127, 144, 128,
        128, 145, 129,
        129, 146, 130,
        130, 147, 131,
        131, 148, 132,
        132, 149, 133,
        133, 150, 134,
        134, 151, 135};

    // cell offset used to iterate neightbor
    inline static const std::vector<int> offsetCells = {
        -1, -1, -1,
        0, -1, -1,
        1, -1, -1,
        -1, 0, -1,
        0, 0, -1,
        1, 0, -1,
        -1, 1, -1,
        0, 1, -1,
        1, 1, -1,
        -1, -1, 0,
        0, -1, 0,
        1, -1, 0,
        -1, 0, 0,
        1, 0, 0,
        -1, 1, 0,
        0, 1, 0,
        1, 1, 0,
        -1, -1, 1,
        0, -1, 1,
        1, -1, 1,
        -1, 0, 1,
        0, 0, 1,
        1, 0, 1,
        -1, 1, 1,
        0, 1, 1,
        1, 1, 1};

    struct SpatialCell
    {
        int key; // key hashed from particle position
        int idx; // index of original particle array

        SpatialCell() : key(-1), idx(-1) {}
        SpatialCell(int k, int s) : key(k), idx(s) {}

        bool operator<(const SpatialCell &idx) const
        {
            return (key < idx.key);
        }
    };

    const int PRIME_X = 73856093;
    const int PRIME_Y = 19349663;
    const int PRIME_Z = 83492791;
    const int BUCKET_SIZE = 100000;

    const float SPHERE_RADIUS = 1.0f;
    const float PI = 3.14159265358979f;
    const unsigned int DIMENSION = 3;

    //======[Particle properties]===========
    std::vector<float> densities;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> predicted_positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> accelerations;
    std::vector<glm::vec4> colors;

    std::vector<SpatialCell> positions_hased; // store hashed position of each particles
    std::vector<int> hash_firstIdx;           // first particle of that groupw

    //=======[adjustable parameters]========
    int N_PARTICLES = 1000;
    float MU = 0.75f;               // viscosity constant
    float MASS = 0.02f;             // particle mass
    float PRESSURE_MULT = 1.8f;     // AKA gas constant
    float SMOOTHING_RADIUS = 1.35f; // for kernel
    float DENSITY_0 = 170.0f;       // rest density

    float RESTITUTION = 0.2f; // for bounding box
    float GRAVITY;

    glm::vec3 SPAWN_POS;
    float SPAWN_GAP = 0.8f;
    bool USE_PREDICTED = false;
    //======================================

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis1;
    std::uniform_real_distribution<float> dis2;

public:
    SPHSolver(float g)
        : SPAWN_POS(glm::vec3(0.0f)),
          gen(rd()),
          dis1(0.0f, this->PI * 2.0f),
          dis2(-1.0f, 1.0f)
    {
        GRAVITY = g;

        densities = std::vector<float>(N_PARTICLES, 0.0f);
        accelerations = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        velocities = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        positions = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        predicted_positions = std::vector<glm::vec3>(positions);
        colors = std::vector<glm::vec4>(N_PARTICLES, glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

        positions_hased.resize(N_PARTICLES);
        hash_firstIdx = std::vector<int>(BUCKET_SIZE, -1);

        // init with 100 particles at world origin
        grid_init_particle(glm::vec3(0.0f, 0.0f, 0.0f), N_PARTICLES, SPAWN_GAP, DIMENSION);
        updateSpatialLookup(USE_PREDICTED ? predicted_positions : positions);

    }

    /**
     * Place particle in square/cube shape
     * - position - frontmost_topleft position of particle
     * - size - dimension length
     * - dimension - place in what dimension
     */
    void grid_init_particle(glm::vec3 position, int total_particles, float gap, int dimension)
    {
        switch (dimension)
        {
        case 2:
        {
            int columns = sqrt((float)total_particles);
            for (int i = 0; i < N_PARTICLES; i++)
            {
                positions[i].x = position.x + (float)(i % columns) * gap;
                positions[i].y = position.y - (float)(i / columns) * gap;
                predicted_positions[i].x = position.x + (float)(i % columns) * gap;
                predicted_positions[i].y = position.y - (float)(i / columns) * gap;
            }
            break;
        }
        case 3:
            int size = cbrtf((float)total_particles);
            for (int i = 0; i < N_PARTICLES; i++)
            {
                int z = i / (size * size);
                int remainder = i % (size * size);
                int y = remainder / size;
                int x = remainder % size;

                positions[i].x = position.x + (float)x * gap;
                positions[i].y = position.y + (float)y * gap;
                positions[i].z = position.z + (float)z * gap;
                predicted_positions[i].x = position.x + (float)x * gap;
                predicted_positions[i].y = position.y + (float)y * gap;
                predicted_positions[i].z = position.z + (float)z * gap;
            }
            break;
        }
    }

    // update simulation step
    void solver_step(float deltaTime, glm::vec3 boxMin = glm::vec3(0.0f), glm::vec3 boxMax = glm::vec3(0.0f))
    {

#pragma omp parallel for
        for (int i = 0; i < densities.size(); i++)
        {
            predicted_positions[i] = positions[i] + (velocities[i] * deltaTime);
        }

        updateSpatialLookup(USE_PREDICTED ? predicted_positions : positions);

#pragma omp parallel for
        for (int i = 0; i < densities.size(); i++)
        {
            densities[i] = calculateDensity(i); // recompute all density
        }

        // accmulate velocity by pressure force and other
#pragma omp parallel for
        for (int i = 0; i < densities.size(); i++)
        {
            glm::vec3 a = (calculatePressureTerm(i) + calculateViscosityTerm(i)) / (densities[i] + 1e-6f);
            a += glm::vec3(0.0f, -GRAVITY, 0.0f);


            // velocities[i] += (a + (glm::vec3(0.0f, -GRAVITY, 0.0f))) * deltaTime;

            // leap frog integration
            velocities[i] += 0.5f * (accelerations[i] + a) * deltaTime;
            accelerations[i] = a;
        }

        setColorByVelocity();

// update position & resolve collision of given bounding box
#pragma omp parallel for
        for (int i = 0; i < densities.size(); i++)
        {
            // positions[i] += velocities[i] * deltaTime;

            // leap fron integration
            positions[i] += (velocities[i] * deltaTime) + (0.5f * accelerations[i] * deltaTime * deltaTime);

            // Check X boundaries
            if (positions[i].x - this->SPHERE_RADIUS < boxMin.x)
            {
                positions[i].x = boxMin.x + this->SPHERE_RADIUS;
                velocities[i].x = -velocities[i].x * this->RESTITUTION;
            }
            else if (positions[i].x + this->SPHERE_RADIUS > boxMax.x)
            {
                positions[i].x = boxMax.x - this->SPHERE_RADIUS;
                velocities[i].x = -velocities[i].x * this->RESTITUTION;
            }

            // Check Y boundaries
            if (positions[i].y - this->SPHERE_RADIUS < boxMin.y)
            {
                positions[i].y = boxMin.y + this->SPHERE_RADIUS;
                velocities[i].y = -velocities[i].y * this->RESTITUTION;
            }
            else if (positions[i].y + this->SPHERE_RADIUS > boxMax.y)
            {
                positions[i].y = boxMax.y - this->SPHERE_RADIUS;
                velocities[i].y = -velocities[i].y * this->RESTITUTION;
            }

            // Check Z boundaries
            if (positions[i].z - this->SPHERE_RADIUS < boxMin.z)
            {
                positions[i].z = boxMin.z + this->SPHERE_RADIUS;
                velocities[i].z = -velocities[i].z * this->RESTITUTION;
            }
            else if (positions[i].z + this->SPHERE_RADIUS > boxMax.z)
            {
                positions[i].z = boxMax.z - this->SPHERE_RADIUS;
                velocities[i].z = -velocities[i].z * this->RESTITUTION;
            }
        }
    }

    /**
     * Reset all particles values to initial state, position
     */
    void resetSimulation()
    {
        densities = std::vector<float>(N_PARTICLES, 0.0f);
        positions = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        predicted_positions = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        velocities = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        accelerations = std::vector<glm::vec3>(N_PARTICLES, glm::vec3(0.0f));
        colors = std::vector<glm::vec4>(N_PARTICLES, glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

        positions_hased.resize(N_PARTICLES);
        hash_firstIdx = std::vector<int>(BUCKET_SIZE, -1);

        grid_init_particle(SPAWN_POS, densities.size(), SPAWN_GAP, DIMENSION);
        updateSpatialLookup(USE_PREDICTED ? predicted_positions : positions);

    }

private:
    //=================[Smoothing Kernel function]===========================
    float smoothingKernel(float distance, float radius)
    {
        float factor = 8.0f / (PI * radius * radius * radius);
        float q = distance / radius;
        if (q <= 0.5f)
        {
            return factor * (6.0f * ((q * q * q) - (q * q)) + 1.0f);
        }
        else if (q <= 1.0f)
        {
            return factor * 2.0f * ((1.0f - q) * (1.0f - q) * (1.0f - q));
        }
        else
        {
            return 0.0f;
        }
    }

    float smoothingKernelDerivative(float distance, float radius)
    {
        float factor = 8.0f / (PI * radius * radius * radius);
        float q = distance / radius;
        if (q <= 0.5f)
        {
            // return (factor * ((18.0f * q * q) - (12.0f * q))) / radius;
            return (6.0f * factor * distance) / (radius * radius) * (3.0f * q - 2.0f);
        }
        else if (q <= 1.0f)
        {
            return -(factor * (6.0f * (1.0f - q) * (1.0f - q))) / radius;
        }
        else
        {
            return 0.0f;
        }
    }

    // general kernel
    float poly6Kernel(float distance, float radius)
    {
        if (distance > radius)
        {
            return 0.0f;
        }
        float x = (radius * radius - distance * distance);
        return (315.0f / (64.0f * PI * powf(radius, 9.0))) * (x * x * x);
    }

    // recommended for preesure force
    float spikyKernel(float distance, float radius)
    {
        if (distance > radius)
        {
            return 0.0f;
        }
        float x = radius - distance;
        return 15.0f / (PI * powf(radius, 6.0f)) * (x * x * x);
    }

    glm::vec3 spikyKernelGradient(glm::vec3 &r_vec, float radius)
    {
        float r_norm = glm::length(r_vec);
        if (r_norm > radius)
        {
            return glm::vec3(0.0f);
        }
        float h6 = radius * radius * radius * radius * radius * radius;
        float x = (radius - r_norm) * (radius - r_norm);

        return (-45.0f / (PI * h6) * x) * (r_vec / r_norm);
    }

    // recommended for viscosity force
    float laplacianViscosityKernel(float distance, float radius)
    {
        if (distance > radius)
        {
            return 0.0f;
        }
        float h6 = radius * radius * radius * radius * radius * radius;
        return (45.0f / (2.0f * PI * h6)) * (radius - distance);
    }

    //====================[properties compute function]==============================
    float calculateDensity(int i)
    {
        float density = MASS * poly6Kernel(0.0f, SMOOTHING_RADIUS);
        glm::vec3 pos_i = USE_PREDICTED ? predicted_positions[i] : positions[i];

        forEachWithinRadius(i, USE_PREDICTED, [&](int j)
                            {
            glm::vec3 pos_j = USE_PREDICTED ? predicted_positions[j] : positions[j];
            density += MASS * poly6Kernel(glm::length(pos_i-pos_j) , SMOOTHING_RADIUS); });

        return density;
    }

    glm::vec3 calculatePressureTerm(int i)
    {
        glm::vec3 force(0.0f);
        glm::vec3 pos_i = USE_PREDICTED ? predicted_positions[i] : positions[i];
        float rho_i = densities[i];
        float p_i = (PRESSURE_MULT * (rho_i - DENSITY_0));

        forEachWithinRadius(i, USE_PREDICTED, [&](int j)
                            {
                                glm::vec3 pos_j = USE_PREDICTED ? predicted_positions[j] : positions[j];
                                glm::vec3 r_vec = pos_i - pos_j;

                                float rho_j = densities[j];
                                float p_j = (PRESSURE_MULT * (rho_j - DENSITY_0));

                                force += MASS * ((p_i + p_j) / 2.0f) * spikyKernelGradient(r_vec, SMOOTHING_RADIUS); });
        return force;
    }

    glm::vec3 calculateViscosityTerm(int i)
    {
        glm::vec3 force(0.0f);
        glm::vec3 pos_i = USE_PREDICTED ? predicted_positions[i] : positions[i];

        forEachWithinRadius(i, USE_PREDICTED, [&](int j)
                            {
            glm::vec3 pos_j = USE_PREDICTED ? predicted_positions[j] : positions[j];
            force += MASS * ((velocities[j] - velocities[i])/(densities[j] + 1e-6f)) * laplacianViscosityKernel(glm::length(pos_j - pos_i) , SMOOTHING_RADIUS); });

        return MU * force;
    }

    glm::vec3 random_direction()
    {
        return glm::normalize(glm::vec3(rand() / (float)RAND_MAX - 0.5f,
                                        rand() / (float)RAND_MAX - 0.5f,
                                        rand() / (float)RAND_MAX - 0.5f));
    }

    //==============[Spatial grid method]====================

    // function to enumurate through particles 'i' using spatial grid
    template <typename Func>
    void forEachWithinRadius(int i, bool use_predicted, Func callback)
    {
        glm::vec3 pos_i = use_predicted ? predicted_positions[i] : positions[i];
        glm::ivec3 cell = positionToGrid(pos_i);
        float sqr_radius = SMOOTHING_RADIUS * SMOOTHING_RADIUS;

        // iterate all surrounding neighbor (3x3x3)
        for (int j = 0; j < offsetCells.size(); j += 3)
        {
            int key = hashGridCell(glm::ivec3(cell.x + (offsetCells[j]), cell.y + (offsetCells[j + 1]), cell.z + (offsetCells[j + 2])));
            int start_idx = hash_firstIdx[key];

            // iterate all particles in bucket
            for (int inCell_idx = start_idx; inCell_idx < positions_hased.size(); inCell_idx++)
            {
                // exit loop if none left
                if (positions_hased[inCell_idx].key != key)
                    break;

                int neighbor_idx = positions_hased[inCell_idx].idx;

                // skip self
                if (i == neighbor_idx)
                    continue;
                glm::vec3 v_dist = (pos_i - (use_predicted ? predicted_positions[neighbor_idx] : positions[neighbor_idx]));
                float sqrDist = glm::dot(v_dist, v_dist);

                // re-check if really within radius
                if (sqrDist <= sqr_radius)
                {
                    callback(neighbor_idx); // Call user-supplied callback
                }
            }
        }
    }

    void updateSpatialLookup(std::vector<glm::vec3> &postitions_arr)
    {
// generate all hashed key for particles
#pragma omp parallel for
        for (int i = 0; i < postitions_arr.size(); i++)
        {
            // turn current position into corresponding grid then hash
            glm::ivec3 cell_pos = positionToGrid(postitions_arr[i]);
            int hashkey = hashGridCell(cell_pos);

            positions_hased[i] = SpatialCell(hashkey, i);
        }

// reset all first index array
#pragma omp parallel for
        for (int i = 0; i < hash_firstIdx.size(); i++)
        {
            hash_firstIdx[i] = -1;
        }

        // ascending sort
        std::sort(positions_hased.begin(), positions_hased.end());

        // determine first occurence of key
#pragma omp parallel for
        for (int i = 0; i < postitions_arr.size(); i++)
        {
            int key = positions_hased[i].key;
            int keyPrev = i == 0 ? -1 : positions_hased[i - 1].key;
            if (key != keyPrev)
            {
                hash_firstIdx[key] = i;
            }
        }
    }

    glm::ivec3 positionToGrid(glm::vec3 &pos)
    {
        return glm::floor(pos / SMOOTHING_RADIUS);
    }

    int hashGridCell(glm::ivec3 cell_pos)
    {
        // hash
        int key = (cell_pos.x * PRIME_X) + (cell_pos.y * PRIME_Y) + (cell_pos.z * PRIME_Z);
        return abs(key) % BUCKET_SIZE;
    }

    //================[particle color]========================
    void setColorByVelocity()
    {
#pragma omp parallel for
        for (int i = 0; i < velocities.size(); i++)
        {
            float vel = abs(glm::length(velocities[i]));
            this->colors[i] = getValueBetweenTwoFixedColors(vel / 10.0f);
        }
    }

    glm::vec4 getValueBetweenTwoFixedColors(float value)
    {
        int aR = 0, aG = 0, aB = 255; // RGB for our 1st color (blue in this case).
        int bR = 255, bG = 0, bB = 0; // RGB for our 2nd color (red in this case).

        int red = (float)(bR - aR) * value + aR;   // Evaluated as -255*value + 255.
        int green = (float)(bG - aG) * value + aG; // Evalglm::vec3(0.0f)uates as 0.
        int blue = (float)(bB - aB) * value + aB;  // Evaluates as 255*value + 0.

        return glm::vec4((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, 1.0f);
    }
};