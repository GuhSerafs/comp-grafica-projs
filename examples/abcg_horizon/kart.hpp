#ifndef KART_HPP_
#define KART_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class OpenGLWindow;

class Kart
{
private:
  friend OpenGLWindow;

  glm::mat4 m_modelMatrix{1.0f};
  float m_angle{0.0f};
  float m_speed{0.0f};
  float m_side{0.0f};

  bool m_acc{false};
  bool m_left{false};
  bool m_right{false};
  bool m_dacc{false};
  glm::vec3 m_position{0.0f, 0.0f, -20.0f};

  void moveKart(float speed, float side);
};

#endif