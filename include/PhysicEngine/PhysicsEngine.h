#include <glm/glm.hpp>
#include <iostream>
#include <PhysicEngine/PhysicsObject.h>
#include <vector>

static const float GRAVITY = 9.8;

class PhysicsEngine
{
public:
    std::vector<PhysicsObject *> objectList;
    std::vector<glm::vec3> *instanceBuffer;
    std::vector<glm::vec4> *colorBuffer;
    int instance_id_counter = 0;

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

    //  iterate through objectList and compute everything
    void update(float deltaTime)
    {
        for (int i = 0; i < objectList.size(); i++)
        {
            (*objectList[i]).updatePhysics(deltaTime);                                       // update in object
            (*instanceBuffer)[(*objectList[i]).getInstanceId()] = (*objectList[i]).position; // replace old with updated one
        }
    };

    //  add PhysicObject to the class "objectList"
    void addPhysicObject(MeshShape shape, glm::vec3 pos , glm::vec4 color)
    {
        PhysicsObject *new_Obj = new PhysicsObject(CUBE, pos , color);
        objectList.push_back(new_Obj);

        instanceBuffer->push_back(new_Obj->position); // add position data to buffer
        colorBuffer->push_back(new_Obj->mesh_color);  // add color data to buffer

        new_Obj->setInstanceId(instance_id_counter++); // iid -> first index of its "position" data in "instance buffer"
    }

private:
    static PhysicsEngine *instance;
};