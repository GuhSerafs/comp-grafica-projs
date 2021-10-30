#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>
#include <glm/vec2.hpp>

enum class Input { Right, Left, Down, Up };
enum class State { Starting, Playing, GameOver, Win };

struct GameData {
  State m_state{State::Playing};
  std::bitset<4> m_input;  // [up, down, left, right]
  std::uint8_t tabuleiro_index{0};
  bool comida_existe{false};
  glm::vec2 coord_comida;
};

#endif