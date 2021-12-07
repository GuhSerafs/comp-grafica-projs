#ifndef TESTAROSSA_HPP_
#define TESTAROSSA_HPP_

#include <vector>

#include "abcg.hpp"
#include "labirinto.hpp"

class Testarossa {
 public:
  void loadCubeTexture(const std::string& path);
  void loadDiffuseTexture(std::string_view path);
  void loadNormalTexture(std::string_view path);
  void loadObj(std::string_view path, bool standardize = true);
  void render(GLint KaLoc, GLint KdLoc, GLint KsLoc) const;
  void setupVAO(GLuint program);
  void terminateGL();
  void drawBody(int proximo, int anterior) const;
  void paintGL(GLuint m_program, glm::mat4 &viewMatrix, glm::mat4 &projMatrix, glm::mat4 &kartMatrix, GLfloat &lightDir, GLfloat &Ia, GLfloat &Id, GLfloat &Is);

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

  [[nodiscard]] glm::vec4 getKa() const { return m_Ka; }
  [[nodiscard]] glm::vec4 getKd() const { return m_Kd; }
  [[nodiscard]] glm::vec4 getKs() const { return m_Ks; }
  [[nodiscard]] float getShininess() const { return m_shininess; }

  [[nodiscard]] bool isUVMapped() const { return m_hasTexCoords; }

  [[nodiscard]] GLuint getCubeTexture() const { return m_cubeTexture; }

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec4 m_Ka{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 m_Kd{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 m_Ks{0.0f, 0.0f, 0.0f, 1.0f};

  float m_shininess{2.5};
  GLuint m_diffuseTexture{};
  GLuint m_normalTexture{};
  GLuint m_cubeTexture{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  void computeNormals();
  void computeTangents();
  void createBuffers();
  void standardize();

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

  const std::vector<int> m_sequencia_objetos = {
    m_fim_chassis, m_fim_limpadores, m_fim_black, m_fim_lanterna_di, m_fim_interior,
    m_fim_grelhas, m_fim_vidros, m_fim_lanterna_tr, m_fim_pneu[0], m_fim_roda[0],
    m_fim_pneu[1], m_fim_roda[1], m_fim_pneu[2], m_fim_roda[2], m_fim_pneu[3], m_fim_roda[3]};
};

#endif