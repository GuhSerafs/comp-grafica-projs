#ifndef COBRINHA_HPP_
#define COBRINHA_HPP_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <random>
#include <list>
#include "abcg.hpp"
#include "gamedata.hpp"

enum Direcao {Cima, Baixo, Esquerda, Direita};

class OpenGLWindow;

class Cobrinha
{
public: 
    // Funções usadas pela OpenGLWindow
    void initializeGL(GLuint program);
    void paintGL();
    void terminateGL();
    void update();
    
    // Funções da heurística do jogo
    glm::vec2 posicao_cabeca();
    void avancar();
    Direcao direcaoCabeca();
    glm::vec2 proxCabeca();
    void restaurarCauda();
    bool sobreporCauda(glm::vec2 posicao_comida);
private: 
    friend OpenGLWindow;

    // Variaveis
    Direcao direcao;
    Direcao new_direcao;
    std::list<glm::vec2> corpo;
    glm::vec2 cauda;
    const int debouncer{75};

    abcg::ElapsedTimer m_elapsedTimer;

    GLuint m_program{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    const glm::vec3 m_color{0.00f, 1.00f, 0.00f};
    const float m_scale{0.05f};
    // Vetor de coordenadas dos vertices de um bloquinho
    const std::vector<glm::vec2> vertices{ glm::vec2(-1, -1), 
                                            glm::vec2(-1, 1), 
                                            glm::vec2(1, 1), 
                                            glm::vec2(1, -1), 
                                            glm::vec2(-1, -1)};

    void desenharQuadrado(glm::vec3 cor);
    void bloco(glm::vec2 pos);
    Direcao sentidoOposto();
    void setDirecao(Direcao dir);
};
#endif