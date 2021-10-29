#ifndef TABULEIRO_HPP_
#define TABULEIRO_HPP_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <random>
#include "abcg.hpp"
#include "gamedata.hpp"

// class OpenGLWindow;
class Tabuleiro
{
public: 
    void initializeGL(GLuint program);
    void paintGL(const GameData &gameData);
    void terminateGL();
    void update(const GameData &gameData);
    
private: 
    std::vector<glm::vec2> borda;
    std::uint8_t borda_index{0};

    GLuint m_program{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    const glm::vec3 m_color{0.00f, 0.00f, 1.00f}; 
    const float m_scale{0.05f};
    // Vetor de coordenadas dos vertices de um bloquinho
    const std::vector<glm::vec2> vertices{ glm::vec2(-1, -1), 
                                            glm::vec2(-1, 1), 
                                            glm::vec2(1, 1), 
                                            glm::vec2(1, -1), 
                                            glm::vec2(-1, -1)};

    void desenharQuadrado(glm::vec3 cor);
    void bloco(glm::vec2 pos);
};
#endif