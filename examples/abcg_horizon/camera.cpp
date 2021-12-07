#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(int width, int height)
{
  m_projMatrix = glm::mat4(1.0f);
  auto aspect{static_cast<float>(width) / static_cast<float>(height)};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 20.0f);
}

void Camera::computeViewMatrix()
{
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(float speed)
{
  glm::vec3 forward = glm::normalize(m_at - m_eye);
  m_eye += forward * speed;
  m_at += forward * speed;
  computeViewMatrix();
}

void Camera::truck(float speed)
{
  glm::vec3 forward = glm::normalize(m_at - m_eye);
  glm::vec3 left = glm::cross(m_up, forward);
  m_at -= left * speed;
  m_eye -= left * speed;
  computeViewMatrix();
}

void Camera::truckVert(float speed)
{
  glm::vec3 forward = glm::normalize(m_at - m_eye);
  glm::vec3 left = glm::cross(m_up, forward);
  glm::vec3 up = glm::cross(left, forward);
  m_at -= up * speed;
  m_eye -= up * speed;
  computeViewMatrix();
}

void Camera::pan(float speed)
{
  glm::mat4 transform{glm::mat4(1.0f)};
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);
  m_at = transform * glm::vec4(m_at, 1.0f);
  computeViewMatrix();
}

float Camera::angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 origin)
{
  glm::vec3 da = glm::normalize(a - origin);
  glm::vec3 db = glm::normalize(b - origin);
  return glm::acos(glm::dot(da, db));
}

void Camera::panVert(float speed)
{
  glm::mat4 transform{glm::mat4(1.0f)};
  glm::vec3 forward = glm::normalize(m_at - m_eye);
  float angle = angleBetween(m_up, forward, glm::vec3(0.0f));
  if ((angle < 0.5f && speed > 0.0f) || (angle > 2.6f && speed < 0.0f))
  {
    return;
  }
  glm::vec3 left = glm::normalize(glm::cross(m_up, forward));
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, left);
  transform = glm::translate(transform, -m_eye);
  m_at = transform * glm::vec4(m_at, 1.0f);
  computeViewMatrix();
}

void Camera::centerKart(glm::vec3 position, float angle)
{
  m_at = position;
  m_eye = position - glm::vec3(1.0f * glm::cos(glm::radians(angle)), -0.5f, -1.0f * glm::sin(glm::radians(angle)));
  computeViewMatrix();
}