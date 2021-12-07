#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <vector>

#include "abcg.hpp"

struct Vertex
{
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};
  glm::vec4 tangent{};

  bool operator==(const Vertex &other) const noexcept
  {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
  }
};

class Labirinto
{
public:
  void loadCubeTexture(const std::string &path);
  void loadDiffuseTexture(std::string_view path);
  void loadNormalTexture(std::string_view path);
  void loadObj(std::string_view path, bool standardize = true);
  void render(int numTriangles = -1) const;
  void setupVAO(GLuint program);
  void terminateGL();
  void paintGL(GLuint m_program, glm::mat4 &viewMatrix, glm::mat4 &projMatrix, GLfloat &lightDir, GLfloat &Ia, GLfloat &Id, GLfloat &Is);

  [[nodiscard]] int getNumTriangles() const
  {
    return static_cast<int>(m_indices.size()) / 3;
  }

  [[nodiscard]] float getShininess() const { return m_shininess; }
  void setShininess(float shininess) { m_shininess = shininess; }

  [[nodiscard]] bool isUVMapped() const { return m_hasTexCoords; }

  [[nodiscard]] GLuint getCubeTexture() const { return m_cubeTexture; }

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};
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
};

#endif