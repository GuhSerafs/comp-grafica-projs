#include "cobrinha.hpp"
#include <cppitertools/itertools.hpp>


void Cobrinha::initializeGL(GLuint program){
    terminateGL();

    corpo.push_back(glm::vec2(3+2, 4));
    corpo.push_back(glm::vec2(3+1, 4));
    corpo.push_back(glm::vec2(3, 4));

    m_program = program;
    desenharQuadrado(m_color);
}

void Cobrinha::desenharQuadrado(glm::vec3 cor)
{
  const std::vector<glm::vec3> vetor_cores{cor, cor, cor, cor, cor, cor};

//   for(int i = 0; i<6; i++){
//     vetor_cores.push_back(cor);
//   }
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

void Cobrinha::paintGL(const GameData &gameData){
    for (const glm::vec2 &coord : corpo) {
        bloco(coord);
    }
}

void Cobrinha::bloco(glm::vec2 pos) {
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

void Cobrinha::terminateGL(){
    abcg::glDeleteBuffers(1, &m_vbo);
    abcg::glDeleteBuffers(1, &m_ebo);
    abcg::glDeleteVertexArrays(1, &m_vao);
}

void Cobrinha::update(const GameData &gameData){
    // Logica do update
    if (gameData.m_input[static_cast<size_t>(Input::Up)]) {
        avancar(Direcao::Cima);
        return;
    }
    if (gameData.m_input[static_cast<size_t>(Input::Down)]) {
        avancar(Direcao::Baixo);
        return;
    }
    if (gameData.m_input[static_cast<size_t>(Input::Left)]) {
        avancar(Direcao::Esquerda);
        return;
    }
    if (gameData.m_input[static_cast<size_t>(Input::Right)]) {
        avancar(Direcao::Direita);
        return;
    }
    
}

glm::vec2 Cobrinha::posicao_cabeca(){
    return corpo.front();
}

void Cobrinha::avancar(Direcao dir){
    glm::vec2 cabeca_old = corpo.front();
    glm::vec2 novo_bloco;

    switch(dir) {
        case Direcao::Cima: 
            novo_bloco = glm::vec2(cabeca_old.x, cabeca_old.y + 1);
            break;
        case Direcao::Baixo: 
            novo_bloco = glm::vec2(cabeca_old.x, cabeca_old.y - 1);
            break;
        case Direcao::Esquerda: 
            novo_bloco = glm::vec2(cabeca_old.x - 1, cabeca_old.y);
            break;
        case Direcao::Direita: 
            novo_bloco = glm::vec2(cabeca_old.x + 1, cabeca_old.y);
            break;
    };

    corpo.push_front(novo_bloco);
    cauda = corpo.back();
    corpo.pop_back();
}

Direcao Cobrinha::direcao_cabeca(){
    return direcao;
}

glm::vec2 Cobrinha::prox_cabeca(Direcao dir){
    const glm::vec2 cabeca = posicao_cabeca();
    Direcao direcao_movt = direcao_cabeca();
    glm::vec2 retorno = cabeca;

    // if (dir != NULL) {
    //     direcao_movt = dir;
    // }
    
    switch (direcao_movt) {
        case Direcao::Cima: 
            retorno = glm::vec2(cabeca.x, cabeca.y - 1);
            break;
        case Direcao::Baixo: 
            retorno = glm::vec2(cabeca.x, cabeca.y + 1);
            break;
        case Direcao::Esquerda: 
            retorno = glm::vec2(cabeca.x - 1, cabeca.y);
            break;
        case Direcao::Direita: 
            retorno = glm::vec2(cabeca.x + 1, cabeca.y);
            break;
    }
    return retorno;
}

void Cobrinha::restaurar_cauda() {
    corpo.push_back(cauda);
}

bool Cobrinha::sobrepor_cauda(glm::vec2 posicao_comida){
    int idx = 0;
    for(const glm::vec2 bloco: corpo) {
        if (bloco == posicao_comida)
            return true;
        else {
            idx++;
            if (idx >= (corpo.size() - 1))
                break;
        }
    }
    return false;
}