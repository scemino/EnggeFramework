#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <sstream>
#include <Graphics/VertexArray.h>

static const char *vertexShaderSource =
    R"(#version 100
precision mediump float;
attribute vec2 a_position;
attribute vec4 a_color;
varying vec4 v_color;

void main(void) {
  v_color = a_color;
  gl_Position = vec4(a_position.xy, 0, 1);
})";

static const char *fragmentShaderSource =
    R"(#version 100
precision mediump float;
varying vec4 v_color;

void main(void) {
  gl_FragColor = v_color;
}
)";

struct Vertex {
  glm::vec2 pos;
  ngf::Color color;
};

enum class VertexAttributeType {
  Byte = GL_BYTE,
  UnsignedByte = GL_UNSIGNED_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Float = GL_FLOAT,
};

struct VertexAttribute {
  const char *name;
  int size;
  VertexAttributeType type;
  bool normalized;
  std::size_t offset;
};

std::array<Vertex, 4> vertices{{{.pos={-1.0f, -1.0f}, .color={1.0f, 0.0f, 0.0f, 1.0f}},
                                {.pos={1.0f, -1.0f}, .color={0.0f, 1.0f, 0.0f, 1.0f}},
                                {.pos={1.0f, 1.0f}, .color={0.0f, 0.0f, 1.0f, 1.0f}},
                                {.pos={-1.0f, 1.0f}, .color={1.0f, 1.0f, 1.0f, 1.0f}}
                               }};
std::array<std::uint16_t, 6> indices{{0, 1, 2, 0, 2, 3}};

std::array<VertexAttribute, 2> attributes{{
                                              "a_position", 2, VertexAttributeType::Float, false, offsetof(Vertex, pos),
                                              "a_color", 4, VertexAttributeType::Float, false, offsetof(Vertex, color)
                                          }};

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="03 - Basic Drawing", .size={640, 480}});
    m_shader.load(vertexShaderSource, fragmentShaderSource);

    m_vao = std::make_unique<ngf::VertexArray>();
    m_vbs = std::make_unique<ngf::VertexBuffer>();
    m_vbs->buffer(ngf::VertexBuffer::Type::Array, vertices.size() * sizeof(Vertex), vertices.data());
    m_vbs->buffer(ngf::VertexBuffer::Type::Element, indices.size() * sizeof(Vertex), indices.data());
  }

  struct Locations {
    static constexpr std::size_t CountMax = 5;
    std::array<int, CountMax> data{};
    std::size_t count{0};
  };

  template<typename T>
  static void draw(const ngf::VertexArray &vao,
                   const ngf::VertexBuffer &buffers,
                   const T &attribs,
                   ngf::Shader *pShader) {
    ngf::VertexArray::bind(&vao);
    ngf::VertexBuffer::bind(&buffers);
    ngf::Shader::bind(pShader);

    Locations locations;
    if (pShader) {
      for (auto info : attribs) {
        auto loc = pShader->getAttributeLocation(info.name);
        if (loc == -1)
          continue;

        locations.data[locations.count++] = loc;
        glEnableVertexAttribArray(loc);
        const void *pointer = reinterpret_cast<const void *>(info.offset);
        glVertexAttribPointer(loc,
                              info.size,
                              static_cast<GLenum>(info.type),
                              info.normalized ? GL_TRUE : GL_FALSE,
                              sizeof(Vertex),
                              pointer);
      }
    }

    ngf::RenderTarget target;
    target.draw(ngf::PrimitiveType::Triangles, ngf::ElementType::UnsignedShort, indices.size());

    for (std::size_t i = 0; i < locations.count; ++i) {
      glDisableVertexAttribArray(locations.data[i]);
    }
    ngf::VertexBuffer::bind(nullptr);
    ngf::VertexArray::bind(nullptr);
    ngf::Shader::bind(nullptr);
  }

  void onRender() override {
    m_window.clear(ngf::Colors::Lightblue);
    draw(*m_vao, *m_vbs, attributes, &m_shader);
    Application::onRender();
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    for (size_t i = 0; i < vertices.size(); ++i) {
      std::ostringstream o;
      o << "Vertex #" << (i + 1);
      if (ImGui::TreeNode(o.str().c_str())) {
        if (ImGui::DragFloat2("Position", glm::value_ptr(vertices[i].pos), 0.01f, -1.0f, 1.0f)) {
          m_vbs->buffer(ngf::VertexBuffer::Type::Array, vertices.size() * sizeof(Vertex), vertices.data());
        }
        if (ngf::ImGui::ColorEdit4("Color", &vertices[i].color)) {
          m_vbs->buffer(ngf::VertexBuffer::Type::Array, vertices.size() * sizeof(Vertex), vertices.data());
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

private:
  ngf::Shader m_shader{};
  std::unique_ptr<ngf::VertexArray> m_vao;
  std::unique_ptr<ngf::VertexBuffer> m_vbs{};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}