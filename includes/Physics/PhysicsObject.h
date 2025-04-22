#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include <Graphic/BaseObject.h>

class PhysicsObject : public BaseObject
{
public:
    PhysicsObject(
        MeshShape shape = SPHERE,
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        float mass = 1.0f,
        float gravity = 9.8f)
        : BaseObject(shape, color), gravity(gravity), mass(1.0f)
    {
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        position = pos;
        radius = 1.0f;
        instance_id = -1;
    };

    void setColorByVelocity()
    {
        float vel = abs(glm::length(this->velocity));
        this->mesh_color = getValueBetweenTwoFixedColors(vel / 30.0f);
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

public:
    bool grounded = false;
    float mass;
    float gravity;
    float radius;
    int instance_id;
    glm::vec3 velocity, acceleration, force;
};