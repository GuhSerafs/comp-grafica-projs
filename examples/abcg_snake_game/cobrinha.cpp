#include "cobrinha.hpp"
#include <cppitertools/itertools.hpp>

// Variaveis
// Direcao direcao;
// std::vector<glm::vec2> corpo;

    // Metodos
    // Cobrinha::Cobrinha(glm::vec2 pos){
    //     corpo = std::list<glm::vec2>(0);
    //     corpo.push_back(glm::vec2(pos.x+2, pos.y));
    //     corpo.push_back(glm::vec2(pos.x+1, pos.y));
    //     corpo.push_back(glm::vec2(pos.x, pos.y));

    //     direcao = Direcao::Direita;
    // }
    
    // desenhar();
    glm::vec2 Cobrinha::posicao_cabeca(){
        return corpo.front();
    }

    void Cobrinha::avancar(Direcao dir){
        if (dir != NULL) {
            direcao = dir;
        }

        glm::vec2 pos_old = posicao_cabeca();
        
        glm::vec2 novo_bloco;

        switch(dir) {
            case Direcao::Cima: 
                novo_bloco = glm::vec2(pos_old.x, pos_old.y - 1);
                break;
            case Direcao::Baixo: 
                novo_bloco = glm::vec2(pos_old.x, pos_old.y + 1);
                break;
            case Direcao::Esquerda: 
                novo_bloco = glm::vec2(pos_old.x - 1, pos_old.y);
                break;
            case Direcao::Direita: 
                novo_bloco = glm::vec2(pos_old.x + 1, pos_old.y);
                break;
        };

        corpo.push_front(novo_bloco);
    }
    Direcao Cobrinha::direcao_cabeca(){
        return direcao;
    }
    glm::vec2 Cobrinha::prox_cabeca(Direcao dir){
        glm::vec2 cabeca = posicao_cabeca();
        Direcao direcao_movt = direcao_cabeca();

        if (dir != NULL) {
            direcao_movt = dir;
        }
        
        switch (direcao_movt) {
            case Direcao::Cima: 
                return glm::vec2(cabeca.x, cabeca.y - 1);
                break;
            case Direcao::Baixo: 
                return glm::vec2(cabeca.x, cabeca.y + 1);
                break;
            case Direcao::Esquerda: 
                return glm::vec2(cabeca.x - 1, cabeca.y);
                break;
            case Direcao::Direita: 
                return glm::vec2(cabeca.x + 1, cabeca.y);
                break;
        }
    }