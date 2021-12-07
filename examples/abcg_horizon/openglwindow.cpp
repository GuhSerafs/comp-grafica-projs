#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::handleEvent(SDL_Event &ev)
{
  if (ev.type == SDL_KEYDOWN)
  {
    if (ev.key.keysym.sym == SDLK_w)
      m_kart.m_acc = true;
    if (ev.key.keysym.sym == SDLK_s)
      m_kart.m_dacc = true;
    if (ev.key.keysym.sym == SDLK_a)
      m_kart.m_left = true;
    if (ev.key.keysym.sym == SDLK_d)
      m_kart.m_right = true;
  }
  if (ev.type == SDL_KEYUP)
  {
    if (ev.key.keysym.sym == SDLK_w)
      m_kart.m_acc = false;
    if (ev.key.keysym.sym == SDLK_s)
      m_kart.m_dacc = false;
    if (ev.key.keysym.sym == SDLK_a)
      m_kart.m_left = false;
    if (ev.key.keysym.sym == SDLK_d)
      m_kart.m_right = false;
  }
}

void OpenGLWindow::initializeGL()
{
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  for (std::string program : m_programNames)
  {
    m_programs.push_back(
        createProgramFromFile(getAssetsPath() + "shaders/" + program + ".vert",
                              getAssetsPath() + "shaders/" + program + ".frag"));
  }
  m_testarossaProgram = m_programs.at(0);
  m_labirintoProgram = m_programs.at(2);

  m_labirinto.loadDiffuseTexture(getAssetsPath() + "maps/labirinto.jpg");
  m_testarossa.loadObj(getAssetsPath() + "testarossa.obj", false);
  m_labirinto.loadObj(getAssetsPath() + "labirinto.obj", false);

  m_testarossa.setupVAO(m_testarossaProgram);
  m_labirinto.setupVAO(m_labirintoProgram);

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::paintGL()
{
  update();

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  m_labirinto.paintGL(m_labirintoProgram, m_camera.m_viewMatrix, m_camera.m_projMatrix, m_lightDir.x, m_Ia.x, m_Id.x, m_Is.x);
  m_testarossa.paintGL(m_testarossaProgram, m_camera.m_viewMatrix, m_camera.m_projMatrix, m_kart.m_modelMatrix, m_lightDir.x, m_Ia.x, m_Id.x, m_Is.x);
  glUseProgram(0);
}

void OpenGLWindow::paintUI()
{
  abcg::OpenGLWindow::paintUI();

  if (m_mostrarMenu)
  {
    auto widgetSize{ImVec2(200, 40)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Shader Select", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Phong", "Blinnphong", "Texture"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader",
                            comboItems.at(currentIndex).c_str()))
      {
        for (auto index : iter::range(comboItems.size()))
        {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
      m_testarossaProgram = m_programs.at(currentIndex);
    }
    ImGui::End();
  }

  if (m_mostrarMenu)
  {
    auto widgetSize{ImVec2(200, 270)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 70));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Light Menu", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      ImGui::Text("Propriedades da Luz");
      ImGui::PushItemWidth(widgetSize.x - 36);
      ImGui::ColorEdit3("Ia", &m_Ia.x, ImGuiColorEditFlags_Float);
      ImGui::ColorEdit3("Id", &m_Id.x, ImGuiColorEditFlags_Float);
      ImGui::ColorEdit3("Is", &m_Is.x, ImGuiColorEditFlags_Float);
      ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    {
      ImGui::Text("Direção");
      ImGui::PushItemWidth(widgetSize.x - 12);
      ImGui::SliderFloat("Light.x", &m_lightDir.x, -500.0f, 500.0f, "x: %.1f");
      ImGui::SliderFloat("Light.y", &m_lightDir.y, -500.0f, 500.0f, "y: %.1f");
      ImGui::SliderFloat("Light.z", &m_lightDir.z, -500.0f, 500.0f, "z: %.1f");
      ImGui::PopItemWidth();
    }

    ImGui::Spacing();

    {
      static float shininess = 100.0f;
      ImGui::Text("Brilho");
      ImGui::PushItemWidth(widgetSize.x - 12);
      ImGui::SliderFloat("Shininess", &shininess, 0.0f, 500.0f, "%.1f");
      ImGui::PopItemWidth();
      m_labirinto.setShininess(shininess);
    }
    ImGui::End();
  }

  {
    auto widgetSize{ImVec2(200, 42)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Menu Check Box", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      ImGui::PushItemWidth(widgetSize.x - 12);
      ImGui::Checkbox("Mostrar Menu", &m_mostrarMenu);
      ImGui::PopItemWidth();
    }
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height)
{
  m_viewportWidth = width;
  m_viewportHeight = height;
  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL()
{
  m_testarossa.terminateGL();
  m_labirinto.terminateGL();
}

void OpenGLWindow::update()
{
  float deltaTime{static_cast<float>(getDeltaTime())};

  const float angle_offset = -90.0f;
  const float aceleracao = 0.04f;
  const float frenagem = 0.12f;
  const float atrito = 0.01f;
  const float velocidade_min = 0.05f;

  deltaTime = deltaTime * 2.5f;

  m_camera.centerKart(m_kart.m_position, m_kart.m_angle + angle_offset);

  // Atualiza a velocidade
  if (m_kart.m_acc == true && m_kart.m_dacc == false)
  {
    // Se a velocidade for maior que 0 e tiver acelerando...
    if (m_kart.m_speed >= 0.0f)
    {
      m_kart.m_speed += aceleracao;
    }
    else
    {
      m_kart.m_speed += frenagem;
    }
    if (m_kart.m_speed > 1.0f)
    {
      m_kart.m_speed = 1.0f;
    }
  }
  else if (m_kart.m_dacc == true && m_kart.m_acc == false)
  {
    // Se a velocidade for maior que 0 e tiver acelerando...
    if (m_kart.m_speed <= 0.0f)
    {
      m_kart.m_speed -= aceleracao;
    }
    else
    {
      m_kart.m_speed -= frenagem;
    }
    if (m_kart.m_speed < -1.0f)
    {
      m_kart.m_speed = -1.0f;
    }
  }
  else
  {
    if (m_kart.m_speed < -velocidade_min)
    {
      m_kart.m_speed += atrito;
    }
    else if (m_kart.m_speed > velocidade_min)
    {
      m_kart.m_speed -= atrito;
    }
    else
    {
      m_kart.m_speed = 0.0f;
    }
  }

  // Atualiza o lado
  if (abs(m_kart.m_speed) > velocidade_min && m_kart.m_left == true && m_kart.m_right == false)
  {
    m_kart.m_side += abs(1.0f - m_kart.m_side) * 0.1f + 0.05f;
    if (m_kart.m_side > 1.0f)
    {
      m_kart.m_side = 1.0f;
    }
  }
  else if (abs(m_kart.m_speed) > velocidade_min && m_kart.m_left == false && m_kart.m_right == true)
  {
    m_kart.m_side -= abs(-1.0f - m_kart.m_side) * 0.1f + 0.05f;
    if (m_kart.m_side < -1.0f)
    {
      m_kart.m_side = -1.0f;
    }
  }
  else
  {
    if (m_kart.m_side < -velocidade_min)
    {
      m_kart.m_side += abs(1.0f - m_kart.m_side) * 0.1f;
    }
    else if (m_kart.m_side > velocidade_min)
    {
      m_kart.m_side -= abs(-1.0f - m_kart.m_side) * 0.1f;
    }
    else
    {
      m_kart.m_side = 0.0f;
    }
  }

  m_kart.moveKart(m_kart.m_speed * deltaTime, m_kart.m_side * deltaTime);
}
