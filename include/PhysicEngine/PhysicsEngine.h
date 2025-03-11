#include <glm/glm.hpp>
#include <iostream>
#include <PhysicEngine/PhysicsObject.h>
#include <vector>

static const float GRAVITY = 9.8;

class PhysicsEngine
{
public:
    std::vector<PhysicsObject*> objectList;
    std::vector<glm::vec3> *instanceBuffer;
    int instance_id_counter = 0;

    PhysicsEngine()
    {
        objectList = std::vector<PhysicsObject*>();
    };

    static PhysicsEngine *getInstance()
    {
        if (instance == nullptr)
        {
            instance = new PhysicsEngine();
        }
        return instance;
    }

    // TODO
    //  update value of every object created
    //  maybe iterate to all PhysicObj and compute
    void update(float deltaTime)
    {
        for (int i = 0; i < objectList.size(); i++)
        {
            (*objectList[i]).updatePhysics(deltaTime);
            (*instanceBuffer)[(*objectList[i]).getInstanceId()] = (*objectList[i]).position;
        }
    };

    //  add PhysicObject to this class objectList
    void addPhysicObject(MeshShape shape, glm::vec3 pos)
    {
        PhysicsObject* new_Obj = new PhysicsObject(CUBE, pos);
        objectList.push_back(new_Obj);
        instanceBuffer->push_back(new_Obj->position);
        new_Obj->setInstanceId(instance_id_counter++); // iid -> first index of its localOrigin data
    }

private:
    static PhysicsEngine *instance;
};