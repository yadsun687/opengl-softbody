#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <iostream>

struct MeshData
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

enum MeshShape
{
    CUBE,
    PLANE,
};

// Cube mesh data
const MeshData cube = {

    //VERTICES
    {// front face
     -0.5f, -0.5f, 0.5f,
     -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, -0.5f, 0.5f,
     // back face
     0.5f, -0.5f, 1.5f,
     0.5f, 0.5f, 1.5f,
     -0.5f, 0.5f, 1.5f,
     -0.5f, -0.5f, 1.5f},

     //INDICES
    {0, 1, 2,
     2, 3, 0,

     // back
     4, 5, 6,
     6, 7, 4,

     // top
     1, 6, 5,
     5, 2, 1,

     // bottom
     0, 7, 4,
     4, 3, 0,

     // right
     3, 2, 5,
     5, 4, 3,

     // left
     7, 6, 1,
     1, 0, 7}
};



class BaseObject
{
public:
    BaseObject(MeshShape shape , glm::vec4 color)
    {
        switch (shape)
        {
        case CUBE:
            mesh_data = cube;
            break;
        default: 
            mesh_data = cube;
            break;
        }
        mesh_color = color;
        localOrigin = glm::mat4(1.0f);
    }

    MeshData mesh_data;
    glm::vec4 mesh_color;
    glm::mat4 localOrigin;

    void rotate(float deg, glm::vec3 axis)
    {
        localOrigin = glm::rotate(localOrigin, glm::radians(deg), axis);
    }
    void translate(glm::vec3 v)
    {
        localOrigin = glm::translate(localOrigin, v);
    }
    void resetTransform()
    {
        localOrigin = glm::mat4(1.0f);
    }

    void loadToBuffer(std::vector<float> &vertList){
        for(int i = 0 ; i < mesh_data.vertices.size() ; i++){
            vertList.push_back(mesh_data.vertices[i]);
            if(i%3 == 2){
                vertList.insert(vertList.end(), { mesh_color.r, mesh_color.g, mesh_color.b, mesh_color.a  });
            }
        }

        for(auto &i : vertList){
            std::cout<<i<<",";
        }
        std::cout<<"\n";
    }

};