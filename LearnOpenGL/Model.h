//
//  Model.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/31.
//

#ifndef Model_h
#define Model_h
#include <iostream>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Program.h"
#include "Mesh.h"

class Model
{
public:
    Model();
    Model(const std::string& file);
    ~Model();
    
    void init();
    void draw(Program* program);
    glm::vec3 getPosition() const { return m_position; }
    void setPosition(const glm::vec3& position) { m_position = position; }
    bool loadFinished() { return m_loadFinished; };
    
private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName);
    
private:
    /// model data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> m_meshes;
    std::string m_path;
    std::string m_resource;
    glm::vec3 m_position;
    std::atomic_bool m_loadFinished;
    std::shared_ptr<Assimp::Importer> m_importer;
    const aiScene* m_scene = nullptr;
};

#endif /* Model_h */
