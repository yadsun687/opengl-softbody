#pragma once
#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cstdlib>
#include <omp.h>

#include <Physics/PhysicsObject.h>

class SPHsolver_old
{
public:
    std::vector<PhysicsObject *> scene_objs;
    std::vector<float> densities;
    const float PI = 3.14159265358979f;
    const int DIMENSION = 3;
    const float H = 2.5f;
    const float DENSITY_0 = 1000.0f;
    const float K = 1.0f;

    SPHsolver(std::vector<PhysicsObject *> &objectList)
    {
        scene_objs = objectList;
        densities = std::vector<float>(scene_objs.size(), 0.0f);
    }

    void solve_sph()
    {
#pragma omp parallel for
        for (int i = 0; i < scene_objs.size(); i++)
        {
            densities[i] = this->compute_density(this->scene_objs, *scene_objs[i]);
        }

#pragma omp parallel for
        for (int i = 0; i < scene_objs.size(); i++)
        {
            scene_objs[i]->force = compute_pressure_force(scene_objs, this->densities, *scene_objs[i]);
        }
        std::cout << "Density[" << 0 << "] = " << densities[0] << "\n";
        std::cout << "Force[" << 0 << "] = " << (scene_objs[0]->force).y << "\n";
    }

    float compute_density(std::vector<PhysicsObject *> &scene_objs, PhysicsObject &this_obj)
    {
        float density = 0.0f;
        // add up influences density from neighbor particle
        for (int i = 0; i < scene_objs.size(); i++)
        {
            // TODO: optimize neighbor in radius finding
            float r = glm::distance(this_obj.position, scene_objs[i]->position);
            if (r > this->H)
                continue;

            float w = this->cubic_spline_kernel(r, this->H);
            density += scene_objs[i]->mass * w;
        }
        return density;
    }
    glm::vec3 compute_pressure_force(std::vector<PhysicsObject *> &scene_objs, std::vector<float> &densities, PhysicsObject &this_obj)
    {
        glm::vec3 sum(0.0f);
        float rho_i = densities[this_obj.instance_id];
        float p_i = this->K * (rho_i - this->DENSITY_0);
        float x_i = p_i / ((rho_i * rho_i) + (float)1e-6);

        for (int j = 0; j < scene_objs.size(); j++)
        {
            if (scene_objs[j]->instance_id == this_obj.instance_id) // skip itself
                continue;
            glm::vec3 r = scene_objs[j]->position - this_obj.position;
            float distSqr = glm::dot(r, r);
            if (distSqr > this->H * this->H) // outside kernel
                continue;
            if (glm::length(r) <= (float)1e-6) // particle too close
                continue;

            float m_j = scene_objs[j]->mass;
            float rho_j = densities[j];
            float p_j = this->K * (rho_j - this->DENSITY_0);
            float x_j = p_j / ((rho_j * rho_j) + (float)1e-6);

            sum += m_j * (x_i + x_j) * cubic_spline_gradient(r, this->H);
        }

        glm::vec3 force = -sum / (rho_i + (float)1e-6);
        if (glm::dot(force, force) >= 1000000.0f)
        {
            return glm::normalize(force) * 1000.0f;
        }
        return -sum / (rho_i + (float)1e-6);
    }

private:
    float cubic_spline_kernel(float r, float h)
    {
        float q = r / h;
        float factor = 8.0f / (this->PI * h * h * h);
        if (q >= 0 && q <= 0.5f)
        {
            return factor * (6.0f * ((q * q * q) - (q * q)) + 1.0f);
        }
        else if (q <= 1.0f)
        {
            return factor * (2.0f * (1.0f - q) * (1.0f - q) * (1.0f - q));
        }
        else
        {
            return 0.0f;
        }
    }
    float cubic_spline_derivative(float r, float h)
    {
        float q = r / h;
        float factor = 8.0f / (this->PI * h * h * h);

        float dw;
        if (q < 0.5f)
        {
            dw = (6.0f * factor * r) / (h * h) * (3.0f * r / h - 2.0f);
        }
        else if (q < 1.0f)
        {
            dw = (-6.0f * factor * (1.0f - q) * (1.0f - q)) / h;
        }
        else
        {
            dw = 0.0f;
        }
        return dw;
    }

    glm::vec3 cubic_spline_gradient(glm::vec3 &r_vec, float h)
    {
        float r = glm::length(r_vec);
        float dw_dr = cubic_spline_derivative(r, h);
        return dw_dr * (r_vec / r);
    }
};