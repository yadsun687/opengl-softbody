#pragma once

#include <GraphicObject/BaseObject.h>
#include <glm/glm.hpp>

class PhysicsObject : public BaseObject
{
public:
    PhysicsObject(MeshShape shape, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4 color, float mass = 1.0f, float gravity = 9.8f) : BaseObject(shape, color), gravity(gravity), mass(1.0f)
    {
        translate(position);
    };

    void updatePhysics(float deltaTime)
    {
        if (!grounded)
        {
            velocity += gravity * deltaTime;
        }
        velocity += acceleration * deltaTime;
        translate(velocity * deltaTime);
    }

    void setVelocity(glm::vec3 vel)
    {
        velocity = vel;
    }

    void setInstanceId(int id){
        instance_id = id;
    }
    int getInstanceId(){
        return instance_id;
    }
private:
    bool grounded = false;
    float mass;
    float gravity;
    int instance_id;
    glm::vec3 velocity, acceleration;
};