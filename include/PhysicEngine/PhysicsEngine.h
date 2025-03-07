#include <glm/glm.hpp>
#include <iostream>
#include <PhysicEngine/PhysicsObject.h>
#include <vector>

static const float GRAVITY = 9.8;

class PhysicsEngine
{
public:
    std::vector<PhysicsObject> objectList;

    PhysicsEngine();
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
    void update(float deltaTime){
        for(PhysicsObject &obj : objectList){
            obj.updatePhysics(deltaTime);
            instanceBuffer[obj.getInstanceId()] = obj.localOrigin;
        }
    };

    // TODO
    //  add PhysicObject to this class variable
    void addPhysicObject()
    {
        PhysicsObject new_Obj(CUBE, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));
        objectList.push_back(new_Obj);

        new_Obj.setInstanceId(instanceBuffer.size() - 1); //iid -> first index of its localOrigin data
        instanceBuffer.push_back(new_Obj.localOrigin);
    };

    void setInstanceBuffer(std::vector<glm::mat4> &buffer)
    {
        instanceBuffer = buffer;
    }

private:
    static PhysicsEngine *instance;
    std::vector<glm::mat4> instanceBuffer;
};