#include "tabuleiro.hpp"
#include <cppitertools/itertools.hpp>


void Tabuleiro::initializeGL(GLuint program){
    terminateGL();

    // (0, 0) -> (18, 0)
    for(int i = 0; i<19; i++){
        borda.push_back(glm::vec2(i, 0));
    }
    // (19, 0) -> (19, 18)
    for(int i = 0; i<19; i++){
        borda.push_back(glm::vec2(19, i));
    }
    // (19, 19) -> (1, 19)
    for(int i = 19; i>0; i--){
        borda.push_back(glm::vec2(i, 19));
    }
    // (0, 19) -> (0, 1)
    for(int i = 19; i>0; i--){
        borda.push_back(glm::vec2(0, i));
    }

    borda_index = 0;
    m_program = program;
    desenharQuadrado(m_color);
}

void Tabuleiro::desenharQuadrado(std::vector<glm::vec3> cor)
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

void Tabuleiro::paintGL(){
    for (int i = 0; i<borda_index; i++){
        bloco(borda.at(i));
    }
}

void Tabuleiro::bloco(glm::vec2 pos) {
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

void Tabuleiro::terminateGL(){
    abcg::glDeleteBuffers(1, &m_vbo);
    abcg::glDeleteBuffers(1, &m_ebo);
    abcg::glDeleteVertexArrays(1, &m_vao);
}

void Tabuleiro::update(){
    borda_index++;
}