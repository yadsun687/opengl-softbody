
#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>

#include <Graphic/GUIManager.h>
#include <Physics/PhysicsObject.h>
#include <Physics/SPHsolver.h>

class PhysicsEngine
{
public:
    const float GRAVITY = 9.81;
    const float RESTITUTION = 0.5f;

    std::vector<PhysicsObject *> objectList;
    std::vector<glm::vec3> *instanceBuffer;
    std::vector<glm::vec4> *colorBuffer;

    SPHSolver *sph_solver;

    glm::vec3 boxPosition, boxMin, boxMax;
    int instance_id_counter = 0;

    bool is_pause = false;

    // TODO: collision spatial grid  { <grid_pos> : array[<object_id>] , ... }
    std::map<glm::vec3, std::vector<int>, bool (*)(const glm::vec3 &, const glm::vec3 &)> partition_grid;

private:
    static PhysicsEngine *instance;

    //=================[METHOD]=================
public:
    PhysicsEngine()
    {
        objectList = std::vector<PhysicsObject *>();
    };

    static PhysicsEngine *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new PhysicsEngine();
        }
        return instance;
    }

    /*
     *  called for each engine time step,
     *  do these in order
     *  - compute particles force using SPH
     *  - check & resolve bounding box collision
     *  - update force,velocity,position (also in graphic buffer)
     *
     *  Then finally move the object
     */
    void update(float deltaTime)
    {
        if (is_pause)
            return;

        if (sph_solver != nullptr)
        {
            sph_solver->solver_step(deltaTime, this->boxMin, this->boxMax);
        }

        // //loop for update physics objects
        // for (int i = 0; i < objectList.size(); i++)
        // {
        //     // objectList[i]->force += GRAVITY;
        //     // objectList[i]->velocity += (objectList[i]->force / objectList[i]->mass) * deltaTime;
        //     // objectList[i]->velocity += this->GRAVITY * deltaTime;
        //     objectList[i]->translate(glm::vec3(objectList[i]->velocity.x, objectList[i]->velocity.y, 0.0f) * deltaTime);
        //     // (*objectList[i]).setColorByVelocity();
        //     // put updated value to buffer
        //     // (*colorBuffer)[(*objectList[i]).instance_id] = (*objectList[i]).mesh_color;  // update color using heatmap
        //     // (*instanceBuffer)[(*objectList[i]).instance_id] = (*objectList[i]).position; // replace old with updated one
        // }
    };

    //  add PhysicObject to the class "objectList"
    void addPhysicObject(MeshShape shape, glm::vec3 pos, glm::vec4 color, float mass)
    {
        PhysicsObject *new_Obj = new PhysicsObject(CUBE, pos, color, mass);
        objectList.push_back(new_Obj);

        instanceBuffer->push_back(new_Obj->position); // add position data to buffer
        colorBuffer->push_back(new_Obj->mesh_color);  // add color data to buffer

        new_Obj->instance_id = instance_id_counter++; // iid -> first index of its "position" data in "instance buffer"
    }

    void initSPH()
    {
        this->sph_solver = new SPHSolver(this->objectList, this->GRAVITY);
    }

    // bounding box area for particles
    void setBoundingBox(float width, float height, float depth, glm::vec3 position)
    {
        this->boxMin = glm::vec3(position.x - (width / 2.0f), position.y - (height / 2.0f), position.z - (depth / 2.0f));
        this->boxMax = glm::vec3(position.x + (width / 2.0f), position.y + (height / 2.0f), position.z + (depth / 2.0f));
        this->boxPosition = position;
    }

    // NOTE: TESTING - NOT USE
    void handleObjectCollision()
    {
        for (auto &grid : partition_grid)
        {
            if (grid.second.size() == 0)
                continue;

            for (int i = 0; i < grid.second.size(); i++)
            {
                for (int j = i + 1; j < grid.second.size(); j++)
                {
                    // check if 2 sphere collide
                    if (glm::distance((objectList[grid.second[i]])->position, (objectList[grid.second[j]])->position) <= 2)
                    {
                        glm::vec3 eq1, eq2;
                        float eq_bot = (objectList[grid.second[i]]->mass + objectList[grid.second[j]]->mass);

                        eq1 = objectList[grid.second[i]]->velocity * ((objectList[grid.second[i]]->mass - objectList[grid.second[j]]->mass) / eq_bot);
                        eq1 += objectList[grid.second[j]]->velocity * ((2 * objectList[grid.second[j]]->mass) / eq_bot);

                        eq2 = objectList[grid.second[i]]->velocity * ((2 * objectList[grid.second[i]]->mass) / eq_bot);
                        eq2 += objectList[grid.second[j]]->velocity * ((objectList[grid.second[i]]->mass - objectList[grid.second[j]]->mass) / eq_bot);

                        objectList[grid.second[i]]->velocity = eq1;
                        objectList[grid.second[j]]->velocity = eq2;
                    }
                }
            }
        }
    }
    void initSpatialGrid()
    {
        // auto comp = [](const glm::vec3& a, const glm::vec3& b) { return glm::lessThan(a,b); };
        partition_grid = std::map<glm::vec3, std::vector<int>, bool (*)(const glm::vec3 &, const glm::vec3 &)>(
            [](const glm::vec3 &a, const glm::vec3 &b)
            {
                if (a.x != b.x)
                    return a.x < b.x;
                if (a.y != b.y)
                    return a.y < b.y;
                return a.z < b.z;
            });

        // empty grid with side length 4
        //  FIXME: current only have grid -100 to 100 on every axis
        //     partition_grid.insert(glm::vec3())
        for (int x = -100; x <= 100; x += 10)
        {
            for (int y = -100; y <= 100; y += 10)
            {
                for (int z = -100; z <= 100; z += 10)
                {
                    partition_grid.insert({glm::vec3(x, y, z), {}});
                }
            }
        }

        for (auto &obj : objectList)
        {
            for (auto &grid : partition_grid)
            {
                // for i=1,2,3:
                // if              c[i] < b_min[i]   then   p[i] = b_min[i]
                // if   b_min[i] < c[i] < b_max[i]   then   p[i] = c[i]
                // if   b_max[i] < c[i]              then   p[i] = b_max[i]
                //
                // if (p[1]-c[1])^2 + (p[2]-c[2])^2 + (p[3]-c[3])^2 < r^2
                // then the box intersects the ball.
                //
                // Otherwise the box doesn't intersect the ball

                float obj_rad = 2.0f;
                glm::vec3 obj_center = obj->position;
                glm::vec3 p = obj_center;
                glm::vec3 grid_minp = grid.first;
                glm::vec3 grid_maxp = grid.first + 10.0f;

                if (obj_center.x < grid_minp.x)
                    p.x = grid_minp.x;
                if (obj_center.x >= grid_minp.x && obj_center.x < grid_maxp.x)
                    p.x = obj_center.x;
                if (obj_center.x >= grid_maxp.x)
                    p.x = grid_maxp.x;

                if (obj_center.y < grid_minp.y)
                    p.y = grid_minp.y;
                if (obj_center.y >= grid_minp.y && obj_center.y < grid_maxp.y)
                    p.y = obj_center.y;
                if (obj_center.y >= grid_maxp.y)
                    p.y = grid_maxp.y;

                if (obj_center.z < grid_minp.z)
                    p.z = grid_minp.z;
                if (obj_center.z >= grid_minp.z && obj_center.z < grid_maxp.z)
                    p.z = obj_center.z;
                if (obj_center.z >= grid_maxp.z)
                    p.z = grid_maxp.z;

                // if sphere inside this grid
                if (glm::distance(obj_center, p) <= 2.0f)
                {
                    grid.second.push_back(obj->instance_id);
                }
            }
        }
    }
};