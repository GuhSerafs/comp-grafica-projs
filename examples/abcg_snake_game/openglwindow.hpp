#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>
#include <glm/vec2.hpp>
#include <random>

#include "abcg.hpp"
#include "cobrinha.hpp"

class OpenGLWindow : public abcg::OpenGLWindow
{
protected:
    void handleEvent(SDL_Event& event) override;
    void initializeGL() override;
    void paintGL() override;
    void paintUI() override;
    void resizeGL(int width, int height) override;
    void terminateGL() override;

private:
    GLuint m_vao{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_program{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    Cobrinha m_cobrinha{glm::vec2(4, 3)};

    std::default_random_engine m_randomEngine;

    int m_delay{20};
    abcg::ElapsedTimer m_elapsedTimer;

    // Cores usadas no jogo
    const std::vector<glm::vec3> m_cor_tabuleiro{ glm::vec3(0.00f, 0.27f, 0.39f),
                                                    glm::vec3(0.00f, 0.64f, 0.91f),
                                                    glm::vec3(0.00f, 1.00f, 1.00f),
                                                    glm::vec3(0.00f, 1.00f, 1.00f),
                                                    glm::vec3(0.00f, 0.64f, 0.91f),
                                                    glm::vec3(0.00f, 0.27f, 0.39f)};

    const std::vector<glm::vec3> m_cor_cobrinha{ glm::vec3(0.00f, 1.00f, 0.00f),
                                                glm::vec3(0.00f, 1.00f, 0.00f),
                                                glm::vec3(0.00f, 1.00f, 0.00f),
                                                glm::vec3(0.00f, 1.00f, 0.00f),
                                                glm::vec3(0.00f, 1.00f, 0.00f),
                                                glm::vec3(0.00f, 1.00f, 0.00f)};

    const std::vector<glm::vec3> m_cor_comida{ glm::vec3(1.00f, 0.00f, 0.00f),
                                                glm::vec3(1.00f, 0.00f, 0.00f),
                                                glm::vec3(1.00f, 0.00f, 0.00f),
                                                glm::vec3(1.00f, 0.00f, 0.00f),
                                                glm::vec3(1.00f, 0.00f, 0.00f),
                                                glm::vec3(1.00f, 0.00f, 0.00f)};

    // Vetor de coordenadas dos vertices de um bloquinho
    const std::vector<glm::vec2> vertices{ glm::vec2(-1, -1), 
                                            glm::vec2(-1, 1), 
                                            glm::vec2(1, 1), 
                                            glm::vec2(1, -1), 
                                            glm::vec2(-1, -1)};

    bool inicializa_jogo{true};
    int status_carregamento{0};

    void renderizar();                     
    void setupModel(int sides);
    void desenharQuadrado(std::vector<glm::vec3> cor);
    void bloco(glm::vec2 pos);
    int setupTabuleiro(int status_carregamento);
};
#endif