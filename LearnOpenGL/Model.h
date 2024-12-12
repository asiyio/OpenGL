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
    
    void draw(Program* program);
    
private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int loadTexture(const std::string& file);
    
private:
    /// model data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> m_meshes;
    std::string m_directory;
};

#endif /* Model_h */
