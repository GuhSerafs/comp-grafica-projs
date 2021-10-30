#ifndef COMIDA_HPP_
#define COMIDA_HPP_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;
class Cobrinha;
class Tabuleiro;

class Comida
{
public: 
    void initializeGL(GLuint program);
    void paintGL();
    void terminateGL();
    void update(const GameData &gameData);
private: 
    friend OpenGLWindow;
    friend Cobrinha;
    friend Tabuleiro;
    // Variaveis
    glm::vec2 m_posicao_comida{7, 7};

    GLuint m_program{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    const glm::vec3 m_color{1.00f, 0.00f, 0.00f};
    const float m_scale{0.05f};
    // Vetor de coordenadas dos vertices de um bloquinho
    const std::vector<glm::vec2> vertices{ glm::vec2(-1, -1), 
                                            glm::vec2(-1, 1), 
                                            glm::vec2(1, 1), 
                                            glm::vec2(1, -1), 
                                            glm::vec2(-1, -1)};

    glm::vec3 m_cor_comida{1.00f, 0.00f, 0.00f};

    void desenharQuadrado(glm::vec3 cor);
    void bloco(glm::vec2 pos);
};
#endif