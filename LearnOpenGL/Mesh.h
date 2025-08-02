//
//  Mesh.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/31.
//

#ifndef Mesh_h
#define Mesh_h

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Program.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> _vetexs, std::vector<unsigned int> _indices, std::vector<Texture> _texture);
    ~Mesh();
    
    void draw(Program* program);
    void init();
    bool isInitialized() { return m_initialized; };

private:
    void setupMesh();
    unsigned int loadTexture(const std::string& file);
    
private:
    std::vector<Vertex> m_vertexs;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;
    
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;

    bool m_initialized;
};


#endif /* Mesh_h */
