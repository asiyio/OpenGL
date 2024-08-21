//
//  Light.hpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/21.
//

#ifndef Light_hpp
#define Light_hpp
#include <glm/glm.hpp>

struct Light
{
    Light()
    {
        ambient = glm::vec3(1.0f, 1.0f, 1.0f);
        diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        specular = glm::vec3(1.0f, 1.0f, 1.0f);
        position = glm::vec3(.0f, .0f, .0f);
    }

    Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position)
    {
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        position = _position;
    }
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 position;
};

#endif /* Light_hpp */
