//
//  Light.hpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/21.
//

#ifndef Light_hpp
#define Light_hpp
#include <glm/glm.hpp>

struct SpotLight
{
    SpotLight(glm::vec3 _position, glm::vec3 _color = glm::vec3(1.f, 1.f, 1.f))
    {
        position = _position;
        color = _color;
    }
    
    bool on = true;

    glm::vec3 position;
    glm::vec3 color;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct FlashLight
{
    FlashLight(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color = glm::vec3(1.f, 1.f, 1.f))
    {
        position = _position;
        direction = _direction;
        color = _color;
    }
    bool on = true;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    
    float cutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));

    float intensity = 4.f;
    float constant = 1.f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};
#endif /* Light_hpp */
