#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>
#include "abcg.hpp"
#include "camera.hpp"
#include "labirinto.hpp"
#include "kart.hpp"
#include "testarossa.hpp"

class OpenGLWindow : public abcg::OpenGLWindow
{
protected:
  void handleEvent(SDL_Event &ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void paintModel(GLuint m_program);

private:
  GLuint m_testarossaProgram{};
  GLuint m_labirintoProgram{};
  std::vector<GLuint> m_programs{};
  std::vector<std::string> m_programNames{"phong", "blinnphong", "texture"};

  int m_viewportWidth{};
  int m_viewportHeight{};
  bool m_mostrarMenu{false};

  Testarossa m_testarossa;
  Labirinto m_labirinto;
  Camera m_camera;
  Kart m_kart;

  // Light and material properties
  glm::vec4 m_lightDir{6.0f, 4.0f, -2.0f, 1.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 1.0f, 1.0f};

  void update();
};

#endif