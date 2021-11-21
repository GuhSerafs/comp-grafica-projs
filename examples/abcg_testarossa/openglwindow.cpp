#include "openglwindow.hpp"
#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Custom specialization of std::hash injected in namespace std
namespace std
{
  template <>
  struct hash<Vertex>
  {
    size_t operator()(Vertex const &vertex) const noexcept
    {
      std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
      return h1;
    }
  };
} // namespace std

void OpenGLWindow::initializeGL()
{
  glClearColor(0, 0, 0, 1);
  y = 0.0f; // variable used to explode and bouncing
  z = 0.0f; // variable used to explode and bouncing

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

  isVisaoExplodida = false;
  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "testarossa.vert",
                                    getAssetsPath() + "testarossa.frag");

  // Load model
  loadModelFromFile(getAssetsPath() + "testarossa.obj");
  standardizeBody();
  m_vertices_ToDraw = m_indices.size();

  // Generate VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::loadModelFromFile(std::string_view path)
{
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

  const auto &attrib{reader.GetAttrib()};
  const auto &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto &shape : shapes)
  {
    // Loop over faces(polygon)
    size_t indexOffset{0};
    for (const auto faceNumber :
         iter::range(shape.mesh.num_face_vertices.size()))
    {
      // Number of vertices composing face f
      std::size_t numFaceVertices{shape.mesh.num_face_vertices[faceNumber]};
      // Loop over vertices in the face
      std::size_t startIndex{};
      for (const auto vertexNumber : iter::range(numFaceVertices))
      {
        // Access to vertex
        tinyobj::index_t index{shape.mesh.indices[indexOffset + vertexNumber]};

        // Vertex coordinates
        startIndex = 3 * index.vertex_index;
        tinyobj::real_t vx = attrib.vertices[startIndex + 0];
        tinyobj::real_t vy = attrib.vertices[startIndex + 1];
        tinyobj::real_t vz = attrib.vertices[startIndex + 2];

        Vertex vertex{};
        vertex.position = {vx, vy, vz};

        // If hash doesn't contain this vertex
        if (hash.count(vertex) == 0)
        {
          // Add this index (size of m_vertices)
          hash[vertex] = m_vertices.size();
          // Add this vertex
          m_vertices.push_back(vertex);
        }

        m_indices.push_back(hash[vertex]);
      }
      indexOffset += numFaceVertices;
    }
  }
}

void OpenGLWindow::standardizeBody()
{
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto &vertex : m_vertices)
  {
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
  for (auto &vertex : m_vertices)
  {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void OpenGLWindow::paintGL()
{
  // Animate angle by 15 degrees per second
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(15.0f) * deltaTime);

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLint posX{glGetUniformLocation(m_program, "posX")};
  GLint posY{glGetUniformLocation(m_program, "posY")};
  GLint posZ{glGetUniformLocation(m_program, "posZ")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);
  glBindVertexArray(m_VAO);

  glUniform1f(posX, x); 
  glUniform1f(posY, y); 
  glUniform1f(posZ, z); 

  // Update uniform variable
  GLint angleLoc{glGetUniformLocation(m_program, "angle")};
  glUniform1f(angleLoc, m_angle);

  // Inicia a sequência desenhando o chassis
  if (isVisaoExplodida)
  {
    glUniform1f(posY, y - m_sequencia_offset_explosao[0]);
  }
  glUniform4f(colorLoc, chassisColor[0], chassisColor[1], chassisColor[2], chassisColor[3]);
  m_vertices_ToDraw = m_fim_chassis;
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, nullptr);

  // Continuação da sequência
  for (long unsigned int index = 1; index < m_sequencia_objetos.size(); index++) {
    if (isVisaoExplodida)
    {
      glUniform1f(posY, y - m_sequencia_offset_explosao[index]);
    }
    drawBody(colorLoc, m_sequencia_objetos[index], m_sequencia_objetos[index-1], colorList[m_sequencia_indices[index]]);
  }

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::drawBody(GLint colorLoc, int proximo, int anterior, glm::vec4 cor)
{
  // Desenha um dos corpos contidos no arquivo .obj de acordo com os índices definidos nas constantes
  glUniform4f(colorLoc, cor[0], cor[1], cor[2], cor[3]);
  m_vertices_ToDraw = proximo - anterior;
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, (void *)(anterior * sizeof(GLuint)));
}

void OpenGLWindow::paintUI()
{
  abcg::OpenGLWindow::paintUI();
  // Create a window for the other widgets
  {
    auto widgetSize{ImVec2(172, 217)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, m_viewportHeight - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      std::vector<std::string> comboItems{"Amarelo", "Cinza", "Limão", "Tamarindo",
                                          "Vermelho", "Roxo", "Ciano", "Azul",
                                          "Verde", "Preto"};

      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Chassis", comboItems.at(chassisColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{chassisColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            chassisColorIndex = index;
            m_sequencia_indices[0] = chassisColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Limpadores", comboItems.at(limpadoresColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{limpadoresColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            limpadoresColorIndex = index;
            m_sequencia_indices[1] = limpadoresColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Lant. Diant.", comboItems.at(lanternaDiColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{lanternaDiColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            lanternaDiColorIndex = index;
            m_sequencia_indices[3] = lanternaDiColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Vidro", comboItems.at(vidrosColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{vidrosColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            vidrosColorIndex = index;
            m_sequencia_indices[6] = vidrosColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Lant. Tras.", comboItems.at(lanternaTrColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{lanternaTrColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            lanternaTrColorIndex = index;
            m_sequencia_indices[7] = lanternaTrColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Pneu", comboItems.at(pneuColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{pneuColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            pneuColorIndex = index;
            m_sequencia_indices[8] = pneuColorIndex;
            m_sequencia_indices[10] = pneuColorIndex;
            m_sequencia_indices[12] = pneuColorIndex;
            m_sequencia_indices[14] = pneuColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Roda", comboItems.at(rodaColorIndex).c_str()))
      {
        for (int index : iter::range(comboItems.size()))
        {
          const bool isSelected{rodaColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
          {
            rodaColorIndex = index;
            m_sequencia_indices[9] = rodaColorIndex;
            m_sequencia_indices[11] = rodaColorIndex;
            m_sequencia_indices[13] = rodaColorIndex;
            m_sequencia_indices[15] = rodaColorIndex;
          }
          if (isSelected)
          {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }

      ImGui::Checkbox("Visão Explodida", &isVisaoExplodida);
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
    }
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height)
{
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL()
{
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}