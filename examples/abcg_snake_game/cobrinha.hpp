#ifndef COBRINHA_HPP_
#define COBRINHA_HPP_

#include <array>
#include <glm/vec2.hpp>
#include <random>
#include <list>
#include "abcg.hpp"

enum Direcao {Cima, Baixo, Esquerda, Direita};

class Cobrinha
{
public: 
    // Variaveis
    Direcao direcao;
    std::list<glm::vec2> corpo;

    // Metodos
    // Cobrinha(glm::vec2 pos);
    void initializeGL(GLuint program);
    void paintGL(const GameData &gameData);
    void terminateGL();
    // desenhar();
    glm::vec2 posicao_cabeca();
    void avancar(Direcao dir);
    Direcao direcao_cabeca();
    glm::vec2 prox_cabeca(Direcao dir);
private: 
    friend OpenGLWindow;

    GLuint m_program{};
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};
};
#endif