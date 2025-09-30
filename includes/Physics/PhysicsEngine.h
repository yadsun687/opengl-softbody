
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
    void update(float deltaTime , bool forced = false)
    {
        if (is_pause && !forced)
            return;

        if (sph_solver != nullptr)
        {
            // sph_solver->solver_step(1.0f/240.0f, this->boxMin, this->boxMax);
        }
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
        this->sph_solver = new SPHSolver(this->GRAVITY);
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

};