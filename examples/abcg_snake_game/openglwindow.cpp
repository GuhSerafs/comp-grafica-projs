#include "openglwindow.hpp"
#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }
}

void OpenGLWindow::initializeGL()
{
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Starting;
  m_gameData.tabuleiro_index = 0;

  m_cobrinha.initializeGL(m_objectsProgram);
  m_tabuleiro.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  // const float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if ((m_gameData.m_state == State::Win || 
      m_gameData.m_state == State::GameOver) &&
      m_elapsedTimer.elapsed() > 5) {
    restart();
    return;
  }
  if (m_gameData.m_state == State::Starting) {
    if (m_elapsedTimer.elapsed() < m_delay / 1000.0) {
      return;
    }
    m_elapsedTimer.restart();
    m_tabuleiro.update(m_gameData);
    m_gameData.tabuleiro_index++;
    if(m_gameData.tabuleiro_index >= 75)
      m_gameData.m_state = State::Playing;
  } 

  if (m_gameData.m_state == State::Playing) {
    if (m_elapsedTimer.elapsed() < m_delay / 1000.0) {
      return;
    }
    m_elapsedTimer.restart();
    m_cobrinha.update(m_gameData);
  } 
  

  // if (m_gameData.m_state == State::Playing) {
  //   checkCollisions();
  //   checkWinCondition();
  // }
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  if (m_gameData.m_state == State::Starting) {
    m_tabuleiro.paintGL(m_gameData);
  } else {
    m_cobrinha.paintGL(m_gameData);
  }
  
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(300, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
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
  abcg::glDeleteProgram(m_objectsProgram);
  m_cobrinha.terminateGL();
}

// void OpenGLWindow::renderizar() {
//   // Render
//   abcg::glBindVertexArray(m_vao);
//   abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 6); // 4 + 2
//   abcg::glBindVertexArray(0);
//   abcg::glUseProgram(0);
// }

// void OpenGLWindow::paintGL()
// {
//   // Check whether to render the next polygon
  
//   if(inicializa_jogo) {
//     int stt = setupTabuleiro(status_carregamento);
//     status_carregamento+= stt;
//   } else if (m_elapsedTimer.elapsed() < m_delay / 1000.0) {
//     return;
//   } else {
//     m_elapsedTimer.restart();
//   }

//   // // Create a regular polygon with a number of sides in the range [3,20]
//   // std::uniform_int_distribution<int> intDist(3, 20);
//   // const auto sides{intDist(m_randomEngine)};
//   // //setupModel(sides);
//   // // desenharQuadrado();

//   // abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//   // abcg::glUseProgram(m_program);

//   // renderizar();
// }

// void OpenGLWindow::paintUI()
// {
//   abcg::OpenGLWindow::paintUI();

//   {
//     const auto widgetSize{ImVec2(200, 72)};
//     ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5,
//                                    m_viewportHeight - widgetSize.y - 5));
//     ImGui::SetNextWindowSize(widgetSize);
//     const auto windowFlags{ImGuiWindowFlags_NoResize |
//                            ImGuiWindowFlags_NoCollapse |
//                            ImGuiWindowFlags_NoTitleBar};
//     ImGui::Begin(" ", nullptr, windowFlags);

//     ImGui::PushItemWidth(140);
//     ImGui::SliderInt("Delay", &m_delay, 0, 200, "%d ms");
//     ImGui::PopItemWidth();

//     if (ImGui::Button("Clear window", ImVec2(-1, 30))) {
//       abcg::glClear(GL_COLOR_BUFFER_BIT);
//     }

//     ImGui::End();
//   }
// }

// void OpenGLWindow::setupModel(int sides)
// {
//   // Release previous resources, if any
//   abcg::glDeleteBuffers(1, &m_vboPositions);
//   abcg::glDeleteBuffers(1, &m_vboColors);
//   abcg::glDeleteVertexArrays(1, &m_vao);

//   // Select random colors for the radial gradient
//   std::uniform_real_distribution<float> rd(0.0f, 1.0f);
//   const glm::vec3 color1{rd(m_randomEngine), rd(m_randomEngine),
//                          rd(m_randomEngine)};
//   const glm::vec3 color2{rd(m_randomEngine), rd(m_randomEngine),
//                          rd(m_randomEngine)};

//   // Minimum number of sides is 3
//   sides = std::max(3, sides);

//   std::vector<glm::vec2> positions(0);
//   std::vector<glm::vec3> colors(0);

//   // Polygon center
//   positions.emplace_back(0, 0);
//   colors.push_back(color1);

//   // Border vertices
//   const auto step{M_PI * 2 / sides};
//   for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
//     positions.emplace_back(std::cos(angle), std::sin(angle));
//     colors.push_back(color2);
//   }

//   // Duplicate second vertex
//   positions.push_back(positions.at(1));
//   colors.push_back(color2);

//   // Generate VBO of positions
//   abcg::glGenBuffers(1, &m_vboPositions);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
//   abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
//                      positions.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // Generate VBO of colors
//   abcg::glGenBuffers(1, &m_vboColors);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
//   abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
//                      colors.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // Get location of attributes in the program
//   const auto positionAttribute{
//       abcg::glGetAttribLocation(m_program, "inPosition")};
//   const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

//   // Create VAO
//   abcg::glGenVertexArrays(1, &m_vao);

//   // Bind vertex attributes to current VAO
//   abcg::glBindVertexArray(m_vao);

//   abcg::glEnableVertexAttribArray(positionAttribute);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
//   abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
//                               nullptr);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   abcg::glEnableVertexAttribArray(colorAttribute);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
//   abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
//                               nullptr);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // End of binding to current VAO
//   abcg::glBindVertexArray(0);
// }

// void OpenGLWindow::desenharQuadrado(glm::vec3 cor)
// {
//   const vector<glm::vec3> vetor_cores{0};

//   for(int i = 0; i<6; i++){
//     vetor_cores.push_back(cor);
//   }
//   // Release previous resources, if any
//   abcg::glDeleteBuffers(1, &m_vboPositions);
//   abcg::glDeleteBuffers(1, &m_vboColors);
//   abcg::glDeleteVertexArrays(1, &m_vao);

//   // Generate VBO of positions
//   abcg::glGenBuffers(1, &m_vboPositions);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
//   abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
//                      vertices.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // Generate VBO of colors
//   abcg::glGenBuffers(1, &m_vboColors);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
//   abcg::glBufferData(GL_ARRAY_BUFFER, vetor_cores.size() * sizeof(glm::vec3),
//                      vetor_cores.data(), GL_STATIC_DRAW);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // Get location of attributes in the program
//   const auto positionAttribute{
//       abcg::glGetAttribLocation(m_program, "inPosition")};
//   const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

//   // Create VAO
//   abcg::glGenVertexArrays(1, &m_vao);

//   // Bind vertex attributes to current VAO
//   abcg::glBindVertexArray(m_vao);

//   abcg::glEnableVertexAttribArray(positionAttribute);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
//   abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
//                               nullptr);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   abcg::glEnableVertexAttribArray(colorAttribute);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
//   abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
//                               nullptr);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

//   // End of binding to current VAO
//   abcg::glBindVertexArray(0);
// }

// void OpenGLWindow::bloco(glm::vec2 pos) {
//   abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//   abcg::glUseProgram(m_program);

//   const float fator = 0.05f;
//   const float offset = -1.0f + fator;
  
//   const glm::vec2 translation{offset + 2*fator*(pos.x), offset + 2*fator*(pos.y)};
//   const GLint translationLocation{
//   abcg::glGetUniformLocation(m_program, "translation")};
//   abcg::glUniform2fv(translationLocation, 1, &translation.x);

//   // Choose a random scale factor (1% to 25%)
//   //std::uniform_real_distribution<float> rd2(0.01f, 0.25f);
//   const auto scale{fator};
//   const GLint scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
//   abcg::glUniform1f(scaleLocation, scale);
// }

// int OpenGLWindow::setupTabuleiro(int status_carregamento) {
//   // Carregar um quadrado
//   static int i_mem{0};
//   switch (status_carregamento) {
//     case 0: 
//       desenharQuadrado(m_cor_tabuleiro);
//       break;
//     case 1 ... 21: 
//       // de (0, 0) -> (18, 0)
//       for (int i = i_mem; i<19; i++) {
//         if (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
//         m_elapsedTimer.restart();
//         bloco(glm::vec2{i, 0});
//         renderizar();
//         i_mem++;
//       }
//       break;
//     case 22: 
//       i_mem = 0;
//       break;
//     case 23 ... 43: 
//       // de (19, 0) -> (19, 18)
//       for (int i = i_mem; i<19; i++) {
//         while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
//         m_elapsedTimer.restart();
//         bloco(glm::vec2{19, i});
//         renderizar();
//         i_mem++;
//       }
//       break;
//     case 44 ... 64: 
//       // de (19, 19) -> (1, 19)
//       for (int i = i_mem; i>0; i--) {
//         while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
//         m_elapsedTimer.restart();
//         bloco(glm::vec2{i, 19});
//         renderizar();
//         i_mem--;
//       }
//       break;
//     case 65: 
//       i_mem = 19;
//       break;
//     case 66 ... 86: 
//       // de (0, 19) -> (0, 1)
//       for (int i = i_mem; i>0; i--) {
//         while (m_elapsedTimer.elapsed() < m_delay / 1000.0) return 0;
//         m_elapsedTimer.restart();
//         bloco(glm::vec2{0, i});
//         renderizar();
//         i_mem--;
//       }
//       break;
//     default: 
//       // Carregar um quadrado
//       desenharQuadrado(m_cor_comida);
//       abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//       abcg::glUseProgram(m_program);
//       bloco(glm::vec2{6, 7});
//       renderizar();

//       // Carregar a cobra (teste)
//       desenharQuadrado(m_cor_cobrinha);
//       abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//       abcg::glUseProgram(m_program);
//       bloco(glm::vec2{4, 4});
//       renderizar();

//       abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//       abcg::glUseProgram(m_program);
//       bloco(glm::vec2{5, 4});
//       renderizar();

//       abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
//       abcg::glUseProgram(m_program);
//       bloco(glm::vec2{6, 4});
//       renderizar();
//   }   
//   return 1;
// }
