//
//  Material.hpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/21.
//

#ifndef Material_hpp
#define Material_hpp
#include <glm/glm.hpp>

struct Material
{
    Material()
    {
        diffuse = 0;
        specular = 0;
        shininess = 1.0f;
    }

    Material(unsigned int _diffuse, unsigned int _specular, float _shininess)
    {
        diffuse = _diffuse;
        specular = _specular;
        shininess = _shininess;
    }
    
    unsigned int diffuse;
    unsigned int specular;
    float shininess;
};

#endif /* Material_hpp */
