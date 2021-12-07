#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class OpenGLWindow;

class Camera
{
public:
  void computeViewMatrix();
  void computeProjectionMatrix(int width, int height);

  void dolly(float speed);
  void truck(float speed);
  void pan(float speed);
  void truckVert(float speed);
  float angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin);
  void panVert(float speed);
  void centerKart(glm::vec3 position, float angle);

private:
  friend OpenGLWindow;

  glm::vec3 m_eye{glm::vec3(0.0f, 0.5f, 2.5f)}; // Camera position
  glm::vec3 m_at{glm::vec3(0.0f, 0.5f, 0.0f)};  // Look-at point
  glm::vec3 m_up{glm::vec3(0.0f, 1.0f, 0.0f)};  // "up" direction

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
};

#endif