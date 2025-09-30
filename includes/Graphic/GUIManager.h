#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <Physics/SPHSolver.h>

class GUIManager
{
public:
    ImGuiIO *io;
    SPHSolver *solver;

    float dummyVal1;

    GUIManager() {};

    GUIManager(GLFWwindow *window) : dummyVal1(25.0f)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        this->io = &ImGui::GetIO();
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    };

    static GUIManager *getInstance()
    {
        return instance;
    }

    void showGUI()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow();
        //===========================================
        ImGui::Begin("Parameters Panel");

        if (solver != nullptr)
        {
            // ImGui::Text("Fluid parameters");
            ImGui::Text("Fluid parameters");
            ImGui::SliderFloat("Mass", &(solver->MASS), 0.01f, 100.0f);
            ImGui::SliderFloat("Pressure Mult", &(solver->PRESSURE_MULT), 0.01f, 100.0f);
            ImGui::SliderFloat("Smoothing Radius", &(solver->SMOOTHING_RADIUS), 0.01f, 2.0f);
            ImGui::SliderFloat("Density", &(solver->DENSITY_0), 1.0f, 1000.0f);
            ImGui::SliderFloat("Viscosity (Mu)", &(solver->MU), 0.0f, 10.0f);
            ImGui::Checkbox("Using predicted position", &(solver->USE_PREDICTED));

            ImGui::Text("Environment");
            ImGui::SliderFloat("Gravity", &(solver->GRAVITY), 0.0f, 100.0f);
            ImGui::SliderFloat("Bounding box dampening", &(solver->RESTITUTION), 0.0f, 1.0f);

            ImGui::Text("Spawning");
            ImGui::SliderInt("No. of Particles", &(solver->N_PARTICLES), 1, 100000);
            ImGui::SliderFloat("Spawning gap", &(solver->SPAWN_GAP), 0.0f, 10.0f);

            ImGui::SliderFloat3("Spawning position", glm::value_ptr(solver->SPAWN_POS), *glm::value_ptr(glm::vec3(-50.0f, -50.0f, -50.0f)), *glm::value_ptr(glm::vec3(50.0f, 50.0f, 50.0f)));

            ImGui::SliderFloat3("Box size min", glm::value_ptr(solver->BOX_MIN), *glm::value_ptr(glm::vec3(-50.0f, -50.0f, -50.0f)), *glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
            ImGui::SliderFloat3("Box size max", glm::value_ptr(solver->BOX_MAX), *glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)), *glm::value_ptr(glm::vec3(50.0f, 50.0f, 50.0f)));
        }

        ImGui::End();
        //===========================================

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    static GUIManager *instance;
};

#endif