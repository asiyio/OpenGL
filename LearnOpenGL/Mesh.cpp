//
//  Mesh.c
//  LearnOpenGL
//
//  Created by asi on 2024/8/31.
//

#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> _vetexs, std::vector<unsigned int> _indices, std::vector<Texture> _texture)
    : m_vertexs(_vetexs), m_indices(_indices), m_textures(_texture)
{
    setup_mesh();
}

Mesh::~Mesh()
{
    
}

void Mesh::draw(Program* program)
{
    if (program == nullptr) return;
    
    unsigned int diffIndex = 1;
    unsigned int specIndex = 1;
    for (int i = 0; i < m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        int num = 0;
        if (m_textures[i].type == "diffuse")
        {
            num = diffIndex++;
        }
        else if (m_textures[i].type == "specular")
        {
            num = specIndex++;
        }
        program->set_uniform1i("material." + m_textures[i].type + std::to_string(num), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (int)m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup_mesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex), &m_vertexs[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}
