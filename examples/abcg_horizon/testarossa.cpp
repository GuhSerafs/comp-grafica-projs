#include "testarossa.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <filesystem>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    const std::size_t h2{std::hash<glm::vec3>()(vertex.normal)};
    const std::size_t h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return h1 ^ h2 ^ h3;
  }
};
}  // namespace std

void Testarossa::initObj() {
  // Setup das variaveis usadas no projeto
  colorList = {
      {1.0f, 0.9f, 0.4f, 1.0f}, // 0 Amarelo
      {0.9f, 0.9f, 0.7f, 1.0f}, // 1 Cinza
      {0.8f, 0.9f, 0.4f, 1.0f}, // 2 Limão
      {0.7f, 0.4f, 0.4f, 1.0f}, // 3 Tamarindo
      {0.7f, 0.1f, 0.1f, 1.0f}, // 4 Vermelho
      {0.5f, 0.4f, 0.7f, 1.0f}, // 5 Roxo
      {0.4f, 0.8f, 0.4f, 1.0f}, // 6 Ciano
      {0.1f, 0.1f, 0.7f, 1.0f}, // 7 Azul
      {0.0f, 0.4f, 0.1f, 1.0f}, // 8 Verde
      {0.3f, 0.3f, 0.3f, 1.0f}  // 9 Preto
  };

  chassisColorIndex = 4;
  chassisColor = colorList[chassisColorIndex];

  limpadoresColorIndex = 1;
  limpadoresColor = colorList[limpadoresColorIndex];

  blackColorIndex = 9;
  blackColor = colorList[blackColorIndex];

  lanternaDiColorIndex = 1;
  lanternaDiColor = colorList[lanternaDiColorIndex];

  interiorColorIndex = 0;
  interiorColor = colorList[interiorColorIndex];

  grelhasColorIndex = 9;
  grelhasColor = colorList[grelhasColorIndex];

  vidrosColorIndex = 1;
  vidrosColor = colorList[vidrosColorIndex];

  lanternaTrColorIndex = 4; 
  lanternaTrColor = colorList[lanternaTrColorIndex];

  pneuColorIndex = 9;
  pneuColor = colorList[pneuColorIndex];

  rodaColorIndex = 0; 
  rodaColor = colorList[rodaColorIndex];

  m_sequencia_indices.clear();
  m_sequencia_indices.push_back(chassisColorIndex);     // 0
  m_sequencia_indices.push_back(limpadoresColorIndex);  // 1
  m_sequencia_indices.push_back(blackColorIndex);       // 2
  m_sequencia_indices.push_back(lanternaDiColorIndex);  // 3
  m_sequencia_indices.push_back(interiorColorIndex);    // 4
  m_sequencia_indices.push_back(grelhasColorIndex);     // 5
  m_sequencia_indices.push_back(vidrosColorIndex);      // 6
  m_sequencia_indices.push_back(lanternaTrColorIndex);  // 7
  m_sequencia_indices.push_back(pneuColorIndex);        // 8
  m_sequencia_indices.push_back(rodaColorIndex);        // 9
  m_sequencia_indices.push_back(pneuColorIndex);        // 10
  m_sequencia_indices.push_back(rodaColorIndex);        // 11
  m_sequencia_indices.push_back(pneuColorIndex);        // 12
  m_sequencia_indices.push_back(rodaColorIndex);        // 13
  m_sequencia_indices.push_back(pneuColorIndex);        // 14
  m_sequencia_indices.push_back(rodaColorIndex);        // 15
}

void Testarossa::computeNormals() {
  // Clear previous vertex normals
  for (auto& vertex : m_vertices) {
    vertex.normal = glm::zero<glm::vec3>();
  }

  // Compute face normals
  for (const auto offset : iter::range<int>(0, m_indices.size(), 3)) {
    // Get face vertices
    Vertex& a{m_vertices.at(m_indices.at(offset + 0))};
    Vertex& b{m_vertices.at(m_indices.at(offset + 1))};
    Vertex& c{m_vertices.at(m_indices.at(offset + 2))};

    // Compute normal
    const auto edge1{b.position - a.position};
    const auto edge2{c.position - b.position};
    glm::vec3 normal{glm::cross(edge1, edge2)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto& vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Testarossa::computeTangents() {
  // Reserve space for bitangents
  std::vector<glm::vec3> bitangents(m_vertices.size(), glm::vec3(0));

  // Compute face tangents and bitangents
  for (const auto offset : iter::range<int>(0, m_indices.size(), 3)) {
    // Get face indices
    const auto i1{m_indices.at(offset + 0)};
    const auto i2{m_indices.at(offset + 1)};
    const auto i3{m_indices.at(offset + 2)};

    // Get face vertices
    Vertex& v1{m_vertices.at(i1)};
    Vertex& v2{m_vertices.at(i2)};
    Vertex& v3{m_vertices.at(i3)};

    const auto e1{v2.position - v1.position};
    const auto e2{v3.position - v1.position};
    const auto delta1{v2.texCoord - v1.texCoord};
    const auto delta2{v3.texCoord - v1.texCoord};

    glm::mat2 M;
    M[0][0] = delta2.t;
    M[0][1] = -delta1.t;
    M[1][0] = -delta2.s;
    M[1][1] = delta1.s;
    M *= (1.0f / (delta1.s * delta2.t - delta2.s * delta1.t));

    const auto tangent{glm::vec4(M[0][0] * e1.x + M[0][1] * e2.x,
                                 M[0][0] * e1.y + M[0][1] * e2.y,
                                 M[0][0] * e1.z + M[0][1] * e2.z, 0.0f)};

    const auto bitangent{glm::vec3(M[1][0] * e1.x + M[1][1] * e2.x,
                                   M[1][0] * e1.y + M[1][1] * e2.y,
                                   M[1][0] * e1.z + M[1][1] * e2.z)};

    // Accumulate on vertices
    v1.tangent += tangent;
    v2.tangent += tangent;
    v3.tangent += tangent;

    bitangents.at(i1) += bitangent;
    bitangents.at(i2) += bitangent;
    bitangents.at(i3) += bitangent;
  }

  for (auto&& [i, vertex] : iter::enumerate(m_vertices)) {
    const auto& n{vertex.normal};
    const auto& t{glm::vec3(vertex.tangent)};

    // Orthogonalize t with respect to n
    const auto tangent{t - n * glm::dot(n, t)};
    vertex.tangent = glm::vec4(glm::normalize(tangent), 0);

    // Compute handedness of re-orthogonalized basis
    const auto b{glm::cross(n, t)};
    const auto handedness{glm::dot(b, bitangents.at(i))};
    vertex.tangent.w = (handedness < 0.0f) ? -1.0f : 1.0f;
  }
}

void Testarossa::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Testarossa::loadCubeTexture(const std::string& path) {
  if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_cubeTexture);
  m_cubeTexture = abcg::opengl::loadCubemap(
      {path + "posx.jpg", path + "negx.jpg", path + "posy.jpg",
       path + "negy.jpg", path + "posz.jpg", path + "negz.jpg"});
}

void Testarossa::loadDiffuseTexture(std::string_view path) {
  if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::opengl::loadTexture(path);
}

void Testarossa::loadNormalTexture(std::string_view path) {
  if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_normalTexture);
  m_normalTexture = abcg::opengl::loadTexture(path);
}

// void Testarossa::loadObj(std::string_view path, bool standardize) {
//   const auto basePath{std::filesystem::path{path}.parent_path().string() + "/"};

//   tinyobj::ObjReaderConfig readerConfig;
//   readerConfig.mtl_search_path = basePath;  // Path to material files

//   tinyobj::ObjReader reader;

//   if (!reader.ParseFromFile(path.data(), readerConfig)) {
//     if (!reader.Error().empty()) {
//       throw abcg::Exception{abcg::Exception::Runtime(
//           fmt::format("Failed to load model {} ({})", path, reader.Error()))};
//     }
//     throw abcg::Exception{
//         abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
//   }

//   if (!reader.Warning().empty()) {
//     fmt::print("Warning: {}\n", reader.Warning());
//   }

//   const auto& attrib{reader.GetAttrib()};
//   const auto& shapes{reader.GetShapes()};
//   const auto& materials{reader.GetMaterials()};

//   m_vertices.clear();
//   m_indices.clear();

//   m_hasNormals = false;
//   m_hasTexCoords = false;

//   // A key:value map with key=Vertex and value=index
//   std::unordered_map<Vertex, GLuint> hash{};

//   // Loop over shapes
//   for (const auto& shape : shapes) {
//     // Loop over indices
//     for (const auto offset : iter::range(shape.mesh.indices.size())) {
//       // Access to vertex
//       const tinyobj::index_t index{shape.mesh.indices.at(offset)};

//       // Vertex position
//       const int startIndex{3 * index.vertex_index};
//       const float vx{attrib.vertices.at(startIndex + 0)};
//       const float vy{attrib.vertices.at(startIndex + 1)};
//       const float vz{attrib.vertices.at(startIndex + 2)};

//       // Vertex normal
//       float nx{};
//       float ny{};
//       float nz{};
//       if (index.normal_index >= 0) {
//         m_hasNormals = true;
//         const int normalStartIndex{3 * index.normal_index};
//         nx = attrib.normals.at(normalStartIndex + 0);
//         ny = attrib.normals.at(normalStartIndex + 1);
//         nz = attrib.normals.at(normalStartIndex + 2);
//       }

//       // Vertex texture coordinates
//       float tu{};
//       float tv{};
//       if (index.texcoord_index >= 0) {
//         m_hasTexCoords = true;
//         const int texCoordsStartIndex{2 * index.texcoord_index};
//         tu = attrib.texcoords.at(texCoordsStartIndex + 0);
//         tv = attrib.texcoords.at(texCoordsStartIndex + 1);
//       }

//       Vertex vertex{};
//       vertex.position = {vx, vy, vz};
//       vertex.normal = {nx, ny, nz};
//       vertex.texCoord = {tu, tv};

//       // If hash doesn't contain this vertex
//       if (hash.count(vertex) == 0) {
//         // Add this index (size of m_vertices)
//         hash[vertex] = m_vertices.size();
//         // Add this vertex
//         m_vertices.push_back(vertex);
//       }

//       m_indices.push_back(hash[vertex]);
//     }
//   }

//   // Use properties of first material, if available
//   if (!materials.empty()) {
//     const auto& mat{materials.at(0)};  // First material
//     m_Ka = glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1);
//     m_Kd = glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1);
//     m_Ks = glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1);
//     m_shininess = mat.shininess;

//     if (!mat.diffuse_texname.empty())
//       loadDiffuseTexture(basePath + mat.diffuse_texname);

//     if (!mat.normal_texname.empty()) {
//       loadNormalTexture(basePath + mat.normal_texname);
//     } else if (!mat.bump_texname.empty()) {
//       loadNormalTexture(basePath + mat.bump_texname);
//     }
//   } else {
//     // Default values
//     m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
//     m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
//     m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
//     m_shininess = 25.0f;
//   }

//   if (standardize) {
//     this->standardize();
//   }

//   if (!m_hasNormals) {
//     computeNormals();
//   }

//   if (m_hasTexCoords) {
//     computeTangents();
//   }

//   createBuffers();
// }
void Testarossa::loadObj(std::string_view path, bool standardize) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Testarossa::paintGL(GLuint program) {
  // update();

  // Clear color buffer and depth buffer
  // abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(program);

  // Get location of uniform variables (could be precomputed)
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(program, "color")};

  abcg::glBindVertexArray(m_VAO);

  // // Draw white bunny
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.2f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  // abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
  
  // abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
  //                      nullptr);
  drawModel(colorLoc);
  

  // Draw yellow bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
  model = glm::scale(model, glm::vec3(0.2f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  // abcg::glUniform4f(colorLoc, 1.0f, 0.8f, 0.0f, 1.0f);
  // abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
  //                      nullptr);
  drawModel(colorLoc);

  // Draw blue bunny
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.2f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  // abcg::glUniform4f(colorLoc, 0.0f, 0.8f, 1.0f, 1.0f);
  // abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
  //                      nullptr);
  drawModel(colorLoc);

  // Draw red bunny
  model = glm::mat4(1.0);
  model = glm::scale(model, glm::vec3(0.1f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  // abcg::glUniform4f(colorLoc, 1.0f, 0.25f, 0.25f, 1.0f);
  // abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
  //                      nullptr);
  drawModel(colorLoc);

  abcg::glBindVertexArray(0);
}

void Testarossa::drawModel(GLint colorLoc) {
  // Inicia a sequência desenhando o chassis
  glUniform4f(colorLoc, chassisColor[0], chassisColor[1], chassisColor[2], chassisColor[3]);
  m_vertices_ToDraw = m_fim_chassis;
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, nullptr);

  // Continuação da sequência
  for (long unsigned int index = 1; index < m_sequencia_objetos.size(); index++) {
    drawBody(colorLoc, m_sequencia_objetos[index], m_sequencia_objetos[index-1], colorList[m_sequencia_indices[index]]);
  }
}

void Testarossa::drawBody(GLint colorLoc, int proximo, int anterior, glm::vec4 cor)
{
  // Desenha um dos corpos contidos no arquivo .obj de acordo com os índices definidos nas constantes
  glUniform4f(colorLoc, cor[0], cor[1], cor[2], cor[3]);
  m_vertices_ToDraw = proximo - anterior;
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, (void *)(anterior * sizeof(GLuint)));
}

// void Testarossa::setupVAO(GLuint program) {
//   // Release previous VAO
//   abcg::glDeleteVertexArrays(1, &m_VAO);

//   // Create VAO
//   abcg::glGenVertexArrays(1, &m_VAO);
//   abcg::glBindVertexArray(m_VAO);

//   // Bind EBO and VBO
//   abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

//   // Bind vertex attributes
//   const GLint positionAttribute{
//       abcg::glGetAttribLocation(program, "inPosition")};
//   if (positionAttribute >= 0) {
//     abcg::glEnableVertexAttribArray(positionAttribute);
//     abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
//                                 sizeof(Vertex), nullptr);
//   }

//   const GLint normalAttribute{abcg::glGetAttribLocation(program, "inNormal")};
//   if (normalAttribute >= 0) {
//     abcg::glEnableVertexAttribArray(normalAttribute);
//     GLsizei offset{sizeof(glm::vec3)};
//     abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
//                                 sizeof(Vertex),
//                                 reinterpret_cast<void*>(offset));
//   }

//   const GLint texCoordAttribute{
//       abcg::glGetAttribLocation(program, "inTexCoord")};
//   if (texCoordAttribute >= 0) {
//     abcg::glEnableVertexAttribArray(texCoordAttribute);
//     GLsizei offset{sizeof(glm::vec3) + sizeof(glm::vec3)};
//     abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
//                                 sizeof(Vertex),
//                                 reinterpret_cast<void*>(offset));
//   }

//   const GLint tangentCoordAttribute{
//       abcg::glGetAttribLocation(program, "inTangent")};
//   if (tangentCoordAttribute >= 0) {
//     abcg::glEnableVertexAttribArray(tangentCoordAttribute);
//     GLsizei offset{sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)};
//     abcg::glVertexAttribPointer(tangentCoordAttribute, 4, GL_FLOAT, GL_FALSE,
//                                 sizeof(Vertex),
//                                 reinterpret_cast<void*>(offset));
//   }

//   // End of binding
//   abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
//   abcg::glBindVertexArray(0);
// }
void Testarossa::setupVAO(GLuint program) {
  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  const GLint positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Testarossa::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Testarossa::terminateGL() {
  //abcg::glDeleteTextures(1, &m_cubeTexture);
  //abcg::glDeleteTextures(1, &m_normalTexture);
  //abcg::glDeleteTextures(1, &m_diffuseTexture);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}