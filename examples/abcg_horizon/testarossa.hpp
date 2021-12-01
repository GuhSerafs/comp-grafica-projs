#ifndef TESTAROSSA_HPP_
#define TESTAROSSA_HPP_

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

class Testarossa
{
public:
    void loadCubeTexture(const std::string &path);
    void loadDiffuseTexture(std::string_view path);
    void loadNormalTexture(std::string_view path);
    void loadObj(std::string_view path, bool standardize = true);
    
    void drawBody(GLint colorLoc, int proximo, int anterior, glm::vec4 cor);
    void drawModel(GLint colorLoc);

    void setupVAO(GLuint program);
    void initObj();
    void paintGL(GLuint program);
    void terminateGL();

    [[nodiscard]] int getNumTriangles() const
    {
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

    std::vector<float> m_sequencia_indices;
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

    int m_vertices_ToDraw{};
};

#endif