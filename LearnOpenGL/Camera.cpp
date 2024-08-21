//
//  Camera.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/10.
//

#include "Camera.h"

Camera Camera::main_camera;

Camera::Camera()
{
    m_speed   = 0.1f;
    m_forward = glm::vec3(0.f, 0.f, -1.0f);
}

Camera::~Camera()
{
    
}

void Camera::init(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    glm::vec3 dir;
    m_position = position;
    m_up       = up;
    dir        = glm::normalize(m_position - target);
    m_right    = glm::normalize(glm::cross(up, dir));
    m_up       = glm::normalize(glm::cross(dir, m_right));
}

void Camera::update()
{
    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

void Camera::move(MOVE_DIRECTION dir)
{
    switch (dir) {
        case FORWARD:
            m_position += m_forward * m_speed;
            break;
        case BACKWARD:
            m_position -= m_forward * m_speed;
            break;
        case LEFT:
            m_position += m_right * m_speed;
            break;
        case RIGHT:
            m_position -= m_right * m_speed;
            break;
        case UP:
            m_position += m_up * m_speed;
            break;
        case DOWN:
            m_position -= m_up * m_speed;
            break;
        default:
            break;
    }
}

void Camera::rotate(glm::vec3 forward)
{
    m_forward = glm::normalize(forward);
    m_right   = glm::normalize(glm::cross(glm::vec3(0.f, 1.0f, 0.f), m_forward));
    m_up      = glm::normalize(glm::cross(m_forward, m_right));
}

glm::mat4 Camera::get_view()
{
    return m_view;
}

glm::vec3 Camera::get_position()
{
    return m_position;
}
