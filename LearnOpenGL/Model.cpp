//
//  Model.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/31.
//

#include "Model.h"
#include "System.hpp"

Model::Model()
{
    m_loadFinished = false;
    m_position = glm::vec3(0.f, 0.f, 0.f);
}

Model::Model(const std::string& file)
{
    m_loadFinished = false;
    m_path = System::resourcePathWithFile(file);
    m_resource = m_path.substr(0, m_path.find_last_of('/') + 1);
    m_position = glm::vec3(0.f, 0.f, 0.f);
}

Model::~Model()
{
    
}

void Model::init()
{
    m_loadFinished = false;
    loadModel(m_path);
    m_loadFinished = true;
}

void Model::draw(Program* program)
{
    if (program == nullptr) return;

    if (m_loadFinished)
    {
        for (int i = 0; i < m_meshes.size(); i++)
        {
            if (!m_meshes[i].isInitialized())
            {
                m_meshes[i].init();
            }
            m_meshes[i].draw(program);
        }
    }
}

void Model::loadModel(const std::string& path)
{
    if (m_importer == nullptr)
    {
        m_importer = std::make_shared<Assimp::Importer>();
    }

    if (m_scene == nullptr)
    {
        m_scene = m_importer->ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
    }
    
    if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
    {
        std::string error = m_importer->GetErrorString();
        std::cout << "ERROR::ASSIMP::" << m_importer->GetErrorString() << std::endl;
        return;
    }
    
    processNode(m_scene->mRootNode, m_scene);
}

void Model::processNode(aiNode* pNode, const aiScene* pScene)
{
    for(unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, pScene));
    }
    for(unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        processNode(pNode->mChildren[i], pScene);
    }
}

Mesh Model::processMesh(aiMesh* pMesh, const aiScene* pScene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 position;
        position.x = pMesh->mVertices[i].x;
        position.y = pMesh->mVertices[i].y;
        position.z = pMesh->mVertices[i].z;
        vertex.position = position;
        
        glm::vec3 normal;
        normal.x = pMesh->mNormals[i].x;
        normal.y = pMesh->mNormals[i].y;
        normal.z = pMesh->mNormals[i].z;
        vertex.normal = normal;
        
        glm::vec2 texCoords(0.f, 0.f);
        if (pMesh->mTextureCoords[0]) {
            texCoords.x = pMesh->mTextureCoords[0][i].x;
            texCoords.y = pMesh->mTextureCoords[0][i].y;
        }
        vertex.texCoords = texCoords;
        
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
    {
        aiFace face = pMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    if (pMesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
        
        static std::vector<std::pair<aiTextureType, std::string>> textureType =
        {
            {aiTextureType_DIFFUSE, "diffuse"},
            {aiTextureType_SPECULAR, "specular"},
            {aiTextureType_EMISSIVE, "emissive"},
            {aiTextureType_HEIGHT, "height"},
            {aiTextureType_NORMALS, "normals"},
            {aiTextureType_SHININESS, "shininess"},
            {aiTextureType_OPACITY, "opacity"},
        };    
        
        for (int i = 0; i < textureType.size(); ++i)
        {
            std::vector<Texture> _textures = loadMaterialTexture(material, textureType[i].first, textureType[i].second);
            textures.insert(textures.end(), _textures.begin(), _textures.end());
        }
    }
    
    Mesh mesh(vertices, indices, textures);
    return mesh;
}

std::vector<Texture> Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.type = typeName;
            texture.path = m_resource + str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}
