#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include <vector>

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void drawBody(GLint colorLoc, int proximo, int anterior, glm::vec4 cor);

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  float x, y, z;
  bool isVisaoExplodida{};

  std::vector<glm::vec4> colorList;
  glm::vec4 chassisColor;
  glm::vec4 limpadoresColor;
  glm::vec4 blackColor;
  glm::vec4 lanternaDiColor;
  glm::vec4 interiorColor;
  glm::vec4 grelhasColor;
  glm::vec4 vidrosColor;
  glm::vec4 lanternaTrColor;
  glm::vec4 pneuColor;
  glm::vec4 rodaColor;

  int chassisColorIndex{};
  int limpadoresColorIndex{};
  int blackColorIndex{};
  int lanternaDiColorIndex{};
  int interiorColorIndex{};
  int grelhasColorIndex{};
  int vidrosColorIndex{};
  int lanternaTrColorIndex{};
  int pneuColorIndex{};
  int rodaColorIndex{};

  float m_angle{};
  int m_vertices_ToDraw{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void standardizeBody();

  // Valores obtidos usando o script testarossa.py
  const int m_fim_chassis = 63705;
  const int m_fim_limpadores = 67569;
  const int m_fim_black = 67809;
  const int m_fim_lanterna_di = 70041;
  const int m_fim_interior = 159519;
  const int m_fim_grelhas = 175824;
  const int m_fim_vidros = 178392;
  const int m_fim_lanterna_tr = 178758;
  const int m_fim_pneu[4] = {190104, 219672, 249240, 278808};
  const int m_fim_roda[4] = {208326, 237894, 267462, 297030};

  const float m_offset_chassis = -0.1;
  const float m_offset_limpadores = -0.6;
  const float m_offset_black = -0.1;
  const float m_offset_lanterna_di = -0.4;
  const float m_offset_interior = -0.1;
  const float m_offset_grelhas = 0.1;
  const float m_offset_vidros = -0.5;
  const float m_offset_lanterna_tr = -0.4;
  const float m_offset_pneu[4] = {0.3, 0.3, 0.3, 0.3};
  const float m_offset_roda[4] = {0.6, 0.6, 0.6, 0.6};

  const std::vector<int> m_sequencia_objetos = {
    m_fim_chassis, m_fim_limpadores, m_fim_black, m_fim_lanterna_di, m_fim_interior, 
    m_fim_grelhas, m_fim_vidros, m_fim_lanterna_tr, m_fim_pneu[0], m_fim_roda[0], 
    m_fim_pneu[1], m_fim_roda[1], m_fim_pneu[2], m_fim_roda[2], m_fim_pneu[3], m_fim_roda[3]
  };

  const std::vector<float> m_sequencia_offset_explosao = {
    m_offset_chassis, m_offset_limpadores, m_offset_black, m_offset_lanterna_di, m_offset_interior, 
    m_offset_grelhas, m_offset_vidros, m_offset_lanterna_tr, m_offset_pneu[0], m_offset_roda[0], 
    m_offset_pneu[1], m_offset_roda[1], m_offset_pneu[2], m_offset_roda[2], m_offset_pneu[3], m_offset_roda[3]
  };

  std::vector<float> m_sequencia_indices;
};

#endif