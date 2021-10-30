#include "openglwindow.hpp"
#include <imgui.h>
#include <cppitertools/itertools.hpp>
#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event)
{
  // Keyboard events
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
    {
      m_cobrinha.setDirecao(Cima);
    }
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
    {
      m_cobrinha.setDirecao(Baixo);
    }
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
    {
      m_cobrinha.setDirecao(Esquerda);
    }
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
    {
      m_cobrinha.setDirecao(Direita);
    }
  }
}

void OpenGLWindow::initializeGL()
{
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr)
  {
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

void OpenGLWindow::restart()
{
  m_gameData.m_state = State::Starting;
  m_gameData.tabuleiro_index = 0;
  m_gameData.comida_existe = false;
  m_gameData.coord_comida = glm::vec2(7, 7);

  m_cobrinha.initializeGL(m_objectsProgram);
  m_tabuleiro.initializeGL(m_objectsProgram);
  m_comida.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update()
{
  // Wait 5 seconds before restarting
  if ((m_gameData.m_state == State::Win ||
       m_gameData.m_state == State::GameOver) &&
      m_elapsedTimer.elapsed() > 2)
  {
    restart();
    return;
  }
  if (m_gameData.m_state == State::Starting)
  {
    if (m_elapsedTimer.elapsed() < m_startup_delay / 1000.0)
    {
      return;
    }
    m_elapsedTimer.restart();
    m_tabuleiro.update();
    m_gameData.tabuleiro_index++;
    if (m_gameData.tabuleiro_index >= 76)
      m_gameData.m_state = State::Playing;
  }

  if (m_gameData.m_state == State::Playing)
  {
    if (m_elapsedTimer.elapsed() < m_delay / 1000.0)
    {
      return;
    }
    m_elapsedTimer.restart();
    if (!m_gameData.comida_existe)
      colocarComida();
    atualizarCobrinha();
    m_comida.update(m_gameData);
  }

  // if (m_gameData.m_state == State::Playing) {
  //   checkCollisions();
  //   checkWinCondition();
  // }
}

void OpenGLWindow::paintGL()
{
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  if (m_gameData.m_state == State::Starting)
  {
    m_tabuleiro.paintGL();
  }
  else
  {
    m_cobrinha.paintGL();
    m_tabuleiro.paintGL();
    if (m_gameData.comida_existe)
      m_comida.paintGL();
  }
}

void OpenGLWindow::paintUI()
{
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

    if (m_gameData.m_state == State::GameOver)
    {
      ImGui::Text("Game Over!");
    }
    else if (m_gameData.m_state == State::Win)
    {
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

void OpenGLWindow::verificarSeComeu()
{
  glm::vec2 cabeca = m_cobrinha.posicao_cabeca();
  if (m_gameData.comida_existe && m_gameData.coord_comida == cabeca)
  {
    m_gameData.comida_existe = false;
    m_cobrinha.restaurarCauda();
  }
}
bool OpenGLWindow::verificarSeEstaViva()
{
  glm::vec2 prox_coord = m_cobrinha.proxCabeca();
  if (m_cobrinha.sobreporCauda(prox_coord))
  {
    return false;
  }
  return (prox_coord.x > 0 && prox_coord.y > 0 && prox_coord.x < 19 && prox_coord.y < 19);
}
void OpenGLWindow::colocarComida()
{
  // Randomly choose a triangle vertex index
  std::uniform_int_distribution<int> intDistribution(1, 18);
  int x{intDistribution(m_randomEngine)};
  int y{intDistribution(m_randomEngine)};

  glm::vec2 nova_comida{x, y};
  while (m_cobrinha.sobreporCauda(nova_comida))
  {
    nova_comida.x = intDistribution(m_randomEngine);
    nova_comida.y = intDistribution(m_randomEngine);
  }

  m_gameData.coord_comida = nova_comida;
  m_gameData.comida_existe = true;
}
void OpenGLWindow::atualizarCobrinha()
{
  if (verificarSeEstaViva())
  {
    m_cobrinha.update();
    verificarSeComeu();
  }
  else
  {
    m_gameData.m_state = State::GameOver;
  }
}