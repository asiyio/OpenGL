//
//  Camera.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/10.
//

#ifndef Camera_h
#define Camera_h

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera();
    ~Camera();
    
    enum MOVE_DIRECTION
    {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
    };
    
    void init(glm::vec3 position, glm::vec3 target, glm::vec3 up);
    void update();
    void updateAngle(float xOffset, float yOffset);
    void move(MOVE_DIRECTION dir);
    glm::mat4 view();
    glm::vec3 pos();
    glm::vec3 forward();
    
private:
    glm::mat4   m_view;
    glm::vec3   m_position;
    glm::vec3   m_target;
    glm::vec3   m_up;
    glm::vec3   m_right;
    glm::vec3   m_forward;
    float       m_speed;
    float       m_pitch;
    float       m_yaw;
    
public:
    static Camera main_camera;
};

#endif /* Camera_h */
