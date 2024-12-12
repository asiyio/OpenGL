//
//  Model.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/31.
//

#include "Model.h"
#include "System.h"
#include <stb/stb_image.h>

Model::Model()
{
    
}

Model::Model(const std::string& file)
{
    std::string path = System::resourcePathWithFile(file);
    loadModel(path);
}

Model::~Model()
{
    
}

void Model::draw(Program* program)
{
    if (program == nullptr) return;
    
    for (int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].draw(program);
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/') + 1);
    processNode(scene->mRootNode, scene);
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
        std::vector<Texture> diffuse = loadMaterialTexture(material, aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuse.begin(), diffuse.end());
        std::vector<Texture> specular = loadMaterialTexture(material, aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), specular.begin(), specular.end());
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
            texture.id = loadTexture(m_directory + std::string(str.data));
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}

unsigned int Model::loadTexture(const std::string& file)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << file << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
