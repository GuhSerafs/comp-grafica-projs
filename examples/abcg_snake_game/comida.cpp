#include "comida.hpp"


void Comida::initializeGL(GLuint program){
    terminateGL();

    m_program = program;
    desenharQuadrado(m_color);
}

void Comida::desenharQuadrado(glm::vec3 cor)
{
  const std::vector<glm::vec3> vetor_cores{cor, cor, cor, cor, cor, cor};

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
  abcg::glBufferData(GL_ARRAY_BUFFER, vetor_cores.size() * sizeof(glm::vec3),
                     vetor_cores.data(), GL_STATIC_DRAW);
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

void Comida::paintGL(){
    bloco(m_posicao_comida);
}

void Comida::bloco(glm::vec2 pos) {
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

  // Render
  abcg::glBindVertexArray(m_vao);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 6); // 4 + 2
  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Comida::terminateGL(){
    abcg::glDeleteBuffers(1, &m_vbo);
    abcg::glDeleteBuffers(1, &m_ebo);
    abcg::glDeleteVertexArrays(1, &m_vao);
}

void Comida::update(const GameData &gameData){
    m_posicao_comida = gameData.coord_comida;
}