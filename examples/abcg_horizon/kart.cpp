#include "kart.hpp"
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>

void Kart::moveKart(float speed, float side)
{
    if (speed > 0)
    {
        if (side != 0)
        {
            m_angle += (side * 30);
        }
        m_position += glm::vec3(speed * glm::sin(glm::radians(m_angle)), 0.0f, speed * glm::cos(glm::radians(m_angle)));
    }
    if (speed < 0)
    {
        if (side != 0)
        {
            m_angle -= (side * 30);
        }
        m_position += glm::vec3(speed * glm::sin(glm::radians(m_angle)), 0.0f, speed * glm::cos(glm::radians(m_angle)));
    }

    glm::mat4 model{1.0f};
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_angle), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.2f));
    m_modelMatrix = model;
}