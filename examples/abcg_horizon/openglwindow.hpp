#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "testarossa.hpp"

// struct Vertex
// {
//   glm::vec3 position;

//   bool operator==(const Vertex &other) const
//   {
//     return position == other.position;
//   }
// };

class OpenGLWindow : public abcg::OpenGLWindow
{
protected:
  void handleEvent(SDL_Event &ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void drawBody(GLint colorLoc, int proximo, int anterior, glm::vec4 cor);
  void drawModel(GLint colorLoc);
  void loadModel(std::string_view path);

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};

  Ground m_ground;
  Testarossa m_testarossa;

  void loadModelFromFile(std::string_view path);
  void update();
};

#endif