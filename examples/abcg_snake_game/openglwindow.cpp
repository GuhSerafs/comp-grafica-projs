#include "openglwindow.hpp"
#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include "abcg.hpp"

void OpenGLWindow::initializeGL()
{
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;

    out vec4 fragColor;

    void main() {
      vec2 newPosition = inPosition * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  // desenharQuadrado(m_cor_tabuleiro);
}

void OpenGLWindow::renderizar() {
  // Render
  abcg::glBindVertexArray(m_vao);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 6); // 4 + 2
  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintGL()
{
  // Check whether to render the next polygon
  
  if(inicializa_jogo) {
    int stt = setupTabuleiro(status_carregamento);
    status_carregamento+= stt;
  } else if (m_elapsedTimer.elapsed() < m_delay / 1000.0) {
    return;
  } else {
    m_elapsedTimer.restart();
  }

  // // Create a regular polygon with a number of sides in the range [3,20]
  // std::uniform_int_distribution<int> intDist(3, 20);
  // const auto sides{intDist(m_randomEngine)};
  // //setupModel(sides);
  // // desenharQuadrado();

  // abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  // abcg::glUseProgram(m_program);

  // renderizar();
}

void OpenGLWindow::paintUI()
{
  abcg::OpenGLWindow::paintUI();

  {
    const auto widgetSize{ImVec2(200, 72)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
                                   m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    const auto windowFlags{ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);

    ImGui::PushItemWidth(140);
    ImGui::SliderInt("Delay", &m_delay, 0, 200, "%d ms");
    ImGui::PopItemWidth();

    if (ImGui::Button("Clear window", ImVec2(-1, 30))) {
      abcg::glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height)
{
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL()
{
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::setupModel(int sides)
{
  // Release previous resources, if any
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);

  // Select random colors for the radial gradient
  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  const glm::vec3 color1{rd(m_randomEngine), rd(m_randomEngine),
                         rd(m_randomEngine)};
  const glm::vec3 color2{rd(m_randomEngine), rd(m_randomEngine),
                         rd(m_randomEngine)};

  // Minimum number of sides is 3
  sides = std::max(3, sides);

  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  // Polygon center
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices
  const auto step{M_PI * 2 / sides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color2);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));
  colors.push_back(color2);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vboPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::desenharQuadrado(std::vector<glm::vec3> cor)
{
  // Release previous resources, if any
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vboPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
                     vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, cor.size() * sizeof(glm::vec3),
                     cor.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::bloco(glm::vec2 pos) {
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  abcg::glUseProgram(m_program);

  const float fator = 0.05f;
  const float offset = -1.0f + fator;
  
  const glm::vec2 translation{offset + 2*fator*(pos.x), offset + 2*fator*(pos.y)};
  const GLint translationLocation{
  abcg::glGetUniformLocation(m_program, "translation")};
  abcg::glUniform2fv(translationLocation, 1, &translation.x);

  // Choose a random scale factor (1% to 25%)
  //std::uniform_real_distribution<float> rd2(0.01f, 0.25f);
  const auto scale{fator};
  const GLint scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
  abcg::glUniform1f(scaleLocation, scale);
}

int OpenGLWindow::setupTabuleiro(int status_carregamento) {
  // Carregar um quadrado
  static int i_mem{0};
  switch (status_carregamento) {
    case 0: 
      desenharQuadrado(m_cor_tabuleiro);
      break;
    case 1 ... 21: 
      // de (0, 0) -> (18, 0)
      for (int i = i_mem; i<19; i++) {
        if (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
        m_elapsedTimer.restart();
        bloco(glm::vec2{i, 0});
        renderizar();
        i_mem++;
      }
      break;
    case 22: 
      i_mem = 0;
      break;
    case 23 ... 43: 
      // de (19, 0) -> (19, 18)
      for (int i = i_mem; i<19; i++) {
        while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
        m_elapsedTimer.restart();
        bloco(glm::vec2{19, i});
        renderizar();
        i_mem++;
      }
      break;
    case 44 ... 64: 
      // de (19, 19) -> (1, 19)
      for (int i = i_mem; i>0; i--) {
        while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
        m_elapsedTimer.restart();
        bloco(glm::vec2{i, 19});
        renderizar();
        i_mem--;
      }
      break;
    case 65: 
      i_mem = 19;
      break;
    case 66 ... 86: 
      // de (0, 19) -> (0, 1)
      for (int i = i_mem; i>0; i--) {
        while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
        m_elapsedTimer.restart();
        bloco(glm::vec2{0, i});
        renderizar();
        i_mem--;
      }
      break;
    default: 
      // Carregar um quadrado
      desenharQuadrado(m_cor_comida);
      abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
      abcg::glUseProgram(m_program);
      bloco(glm::vec2{6, 7});
      renderizar();

      // Carregar a cobra (teste)
      desenharQuadrado(m_cor_cobrinha);
      abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
      abcg::glUseProgram(m_program);
      bloco(glm::vec2{4, 4});
      renderizar();

      abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
      abcg::glUseProgram(m_program);
      bloco(glm::vec2{5, 4});
      renderizar();

      abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
      abcg::glUseProgram(m_program);
      bloco(glm::vec2{6, 4});
      renderizar();
  }   
  return 1;
}
