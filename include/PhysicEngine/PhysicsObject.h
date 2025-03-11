#pragma once

#include <GraphicObject/BaseObject.h>
#include <glm/glm.hpp>
#include <iostream>

class PhysicsObject : public BaseObject
{
public:
    PhysicsObject(MeshShape shape = CUBE, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float mass = 1.0f, float gravity = 9.8f) : BaseObject(shape, color), gravity(gravity), mass(1.0f)
    {
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        position = glm::vec3(0.0f);
        instance_id = -1;

        translate(pos);
    };

    void updatePhysics(float deltaTime)
    {
        
        if (true || !detectBoundary(glm::vec3(0.0f , -10.0f , 0.0f) , glm::vec3(0.0f , -10.0f , 0.0f)))
        {
            velocity.y += -gravity * deltaTime;
        }
        else{
            velocity = -velocity * 0.8f;
        }
        velocity += acceleration * deltaTime;
        translate(velocity * deltaTime);
        
        // std::cout<<"Object["<<instance_id<<"]:"<<position.x<<","<<position.y<<","<<position.z<<"\n";
    }

    bool detectBoundary(glm::vec3 start , glm::vec3 stop){
        float floor = std::min(start.y , stop.y);
        if(position.y > floor && (double)std::abs(position.y - floor) < 1e-2){
            return true;
        }
        return false;
    }

    void setVelocity(glm::vec3 vel)
    {
        velocity = vel;
    }

    void setInstanceId(int id)
    {
        instance_id = id;
    }
    int getInstanceId()
    {
        return instance_id;
    }

private:
    bool grounded = false;
    float mass;
    float gravity;
    int instance_id;
    glm::vec3 velocity, acceleration;
};