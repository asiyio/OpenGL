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
    m_speed   = 0.02f;
    m_pitch   = 0.0f;
    m_yaw     = -90.f;
    m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
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

void Camera::updateAngle(float xOffset, float yOffset)
{
    m_yaw   += xOffset;
    m_pitch += yOffset;

    m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward.y = sin(glm::radians(m_pitch));
    m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_right     = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_forward));
    m_up        = glm::normalize(glm::cross(m_forward, m_right));
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

glm::mat4 Camera::view()
{
    return m_view;
}

glm::vec3 Camera::pos()
{
    return m_position;
}

glm::vec3 Camera::forward()
{
    return m_forward;
}
