#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>
#include <random>
#include "abcg.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "gamedata.hpp"
#include "cobrinha.hpp"
#include "tabuleiro.hpp"
#include "comida.hpp"


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
    GLuint m_objectsProgram{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    GameData m_gameData;
    Cobrinha m_cobrinha;
    Tabuleiro m_tabuleiro;
    Comida m_comida;

    int m_delay{100};
    abcg::ElapsedTimer m_elapsedTimer;

    ImFont* m_font{};
    std::default_random_engine m_randomEngine;

    GLuint m_vao{};
    GLuint m_vboPositions{};
    GLuint m_vboColors{};
    GLuint m_program{};

    void restart();
    void update();

    void verificarSeComeu();
    bool verificarSeEstaViva(Direcao dir);
    void colocarComida();
    void atualizarCobrinha(Direcao dir);

};
#endif