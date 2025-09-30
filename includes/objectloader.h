#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <glm/glm.hpp>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>

struct Constraint
{
    std::pair<unsigned int, unsigned int> pair;
    float distance;
};

struct SoftBodyMesh
{
    std::vector<glm::vec3> vertices;
    // std::vector<glm::vec3> vertices_zero;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> velocities;
    std::vector<unsigned int> faces;
    std::vector<Constraint> structuralPairs;
    std::vector<Constraint> bendPairs;
    glm::vec3 x_cm_zero;
    std::vector<glm::vec3> x_offset_zero;
};

class ObjectLoader
{
public:
    static bool loadOBJ(const std::string &filepath, SoftBodyMesh &outMesh)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << filepath << std::endl;
            return false;
        }
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec3> temp_normals;
        std::set<std::pair<unsigned int, unsigned int>> uniqueStructuralPairs;
        std::set<std::pair<unsigned int, unsigned int>> uniqueBendPairs;
        // std::vector<Vec2> temp_texcoords; // Uncomment if you have texture coordinates

        // A map to ensure we only create unique vertices.
        // The key is a string like "v_idx/vt_idx/vn_idx".
        // The value is the index in our final `outMesh.vertices` vector.
        // std::map<std::string, uint32_t> uniqueVertices;

        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v")
            { // Vertex position
                glm::vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                outMesh.vertices.push_back(pos);
                // outMesh.vertices_zero.push_back(pos);
            }
            else if (prefix == "vn")
            { // Vertex normal
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                outMesh.normals.push_back(normal);
            }
            else if (prefix == "vt")
            { // Vertex texture coordinate
                glm::vec2 tex;
                ss >> tex.x >> tex.y;
                outMesh.texCoords.push_back(tex);
            }
            else if (prefix == "f")
            {
                // Face
                std::string faceVertexStr;
                //!! support only triangulated v/vn format
                unsigned int v[3];

                for (size_t i = 0; i < 3; i++)
                {
                    ss >> faceVertexStr;
                    std::cout << faceVertexStr << " | ";
                    size_t slashPos = faceVertexStr.find("/");
                    size_t slashPos2 = faceVertexStr.find("/", slashPos + 1);
                    if (slashPos != std::string::npos)
                    {
                        int vertexIndex = std::stoi(faceVertexStr.substr(0, slashPos)) - 1;
                        int texIndex = std::stoi(faceVertexStr.substr(slashPos + 1, slashPos2)) - 1;
                        int normalIndex = std::stoi(faceVertexStr.substr(slashPos2 + 1)) - 1;
                        std::cout << texIndex << "\n";
                        outMesh.faces.push_back(vertexIndex);
                        outMesh.faces.push_back(normalIndex);
                        outMesh.faces.push_back(texIndex);

                        v[i] = vertexIndex;
                    }

                    // Handle other face formats (e.g., v/vt/vn, v/vt, v) if needed
                }
                // --- Generate Structural Springs ---
                uniqueStructuralPairs.insert(make_canonical_spring(v[0], v[1]));
                uniqueStructuralPairs.insert(make_canonical_spring(v[1], v[2]));
                uniqueStructuralPairs.insert(make_canonical_spring(v[2], v[0]));
            }
        }

        for (auto &pair : uniqueStructuralPairs)
        {
            outMesh.structuralPairs.push_back(
                {{pair.first, pair.second},
                 glm::distance(outMesh.vertices[pair.first], outMesh.vertices[pair.second])});
        }
        // outMesh.structuralPairs.assign(uniqueStructuralPairs.begin(), uniqueStructuralPairs.end());

        // --- Generate Bend Springs ---
        // Map from an edge to a list of triangles (by index) that share it
        std::map<std::pair<unsigned int, unsigned int>, std::vector<size_t>> edgeToFaceMap;
        for (size_t i = 0; i < outMesh.faces.size(); i += 3)
        {
            unsigned int v1 = outMesh.faces[i];
            unsigned int v2 = outMesh.faces[i + 1];
            unsigned int v3 = outMesh.faces[i + 2];

            size_t triangleIndex = i / 3;
            edgeToFaceMap[make_canonical_spring(v1, v2)].push_back(triangleIndex);
            edgeToFaceMap[make_canonical_spring(v2, v3)].push_back(triangleIndex);
            edgeToFaceMap[make_canonical_spring(v3, v1)].push_back(triangleIndex);
        }

        for (const auto &pair : edgeToFaceMap)
        {
            // Find edges shared by exactly two triangles
            if (pair.second.size() == 2)
            {
                const std::pair<unsigned int, unsigned int> &edge = pair.first;

                // Get the two triangles sharing this edge
                const unsigned int *tri1 = &outMesh.faces[pair.second[0] * 3];
                const unsigned int *tri2 = &outMesh.faces[pair.second[1] * 3];

                // Find the third vertex in each triangle (the ones not on the shared edge)
                unsigned int v3 = find_third_vertex(tri1, edge.first, edge.second);
                unsigned int v4 = find_third_vertex(tri2, edge.first, edge.second);

                // Add the bend spring
                if (v3 == v4)
                    continue;
                uniqueBendPairs.insert(make_canonical_spring(v3, v4));
            }
        }

        // for (auto &pair : uniqueBendPairs)
        // {
        //     outMesh.bendPairs.push_back(
        //         {{pair.first, pair.second},
        //          glm::distance(outMesh.vertices[pair.first], outMesh.vertices[pair.second])});
        // }
        // outMesh.bendPairs.assign(uniqueBendPairs.begin(), uniqueBendPairs.end());

        outMesh.velocities = std::vector<glm::vec3>(outMesh.vertices.size(), glm::vec3(0.0f));

        // for (auto &i : outMesh.structuralPairs)
        // {
        //     std::cout << i.pair.first << ", " << i.pair.second << "| dst: " << i.distance << "\n";
        // }
        // for (auto &i : outMesh.bendPairs)
        // {
        //     std::cout << i.pair.first << ", " << i.pair.second << "| dst: " << i.distance << "\n";
        // }

        // find initial center of mass (x_cm_zero)
        glm::vec3 xcm0(0.0f);
        for (auto &vert : outMesh.vertices)
        {
            xcm0 += vert;
        }
        xcm0 /= outMesh.vertices.size();
        outMesh.x_cm_zero = xcm0;

        // initial offset from center of mass ( x_offset_zero)
        outMesh.x_offset_zero = std::vector<glm::vec3>(outMesh.vertices);
        for (auto &p : outMesh.x_offset_zero)
        {
            p -= outMesh.x_cm_zero;
        }

        file.close();
        return true;
    }

private:
    // Helper function to create a canonical (sorted) pair for edges/springs
    static std::pair<unsigned int, unsigned int> make_canonical_spring(unsigned int v1, unsigned int v2)
    {
        return {std::min(v1, v2), std::max(v1, v2)};
    }
    // Function to find the third vertex in a triangle given two
    static unsigned int find_third_vertex(const unsigned int *triangle, unsigned int v1, unsigned int v2)
    {
        if (triangle[0] != v1 && triangle[0] != v2)
            return triangle[0];
        if (triangle[1] != v1 && triangle[1] != v2)
            return triangle[1];
        return triangle[2];
    }
};

#endif