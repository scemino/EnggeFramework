#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Graphics/Transform.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <sstream>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="04 - Texture", .size={640, 480}});
    m_texture = std::make_unique<ngf::Texture>();
    m_texture->load("./assets/background.jpg");
  }

  void onRender(ngf::RenderTarget &target, const ngf::RenderStates& states) override {
    target.clear(ngf::Colors::Lightblue);
    ngf::RenderStates s;
    s.transform *= m_transform.getTransform();
    s.texture = m_texture.get();
    target.draw(m_primitiveType,
                m_vertices.data(),
                m_vertices.size(),
                Indices.data(),
                Indices.size(),
                s);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    const char *items = {"Points\0LineStrip\0LineLoop\0Lines\0TriangleStrip\0TriangleFan\0Triangles\0\0"};
    auto primitiveIndex = static_cast<int>(m_primitiveType);
    if (ImGui::Combo("Primitive type", &primitiveIndex, items)) {
      m_primitiveType = static_cast<ngf::PrimitiveType>(primitiveIndex);
    }

    auto origin = m_transform.getOrigin();
    if (ImGui::DragFloat2("Origin", glm::value_ptr(origin), 0.1f)) {
      m_transform.setOrigin(origin);
    }
    auto pos = m_transform.getPosition();
    if (ImGui::DragFloat2("Position", glm::value_ptr(pos), 0.1f)) {
      m_transform.setPosition(pos);
    }
    auto rotation = m_transform.getRotation();
    if (ImGui::DragFloat("Rotation", &rotation, 1.f, 0, 360.0f)) {
      m_transform.setRotation(rotation);
    }
    auto scale = m_transform.getScale();
    if (ImGui::DragFloat2("Scale", glm::value_ptr(scale), 0.1f, 0.1f, 10.f)) {
      m_transform.setScale(scale);
    }
    for (size_t i = 0; i < m_vertices.size(); ++i) {
      std::ostringstream o;
      o << "Vertex #" << (i + 1);
      if (ImGui::TreeNode(o.str().c_str())) {
        ImGui::DragFloat2("Position", glm::value_ptr(m_vertices[i].pos), 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat2("Texture coordinates", glm::value_ptr(m_vertices[i].texCoords), 0.01f, 0.0f, 1.0f);
        ngf::ImGui::ColorEdit4("Color", &m_vertices[i].color);
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

private:
  ngf::Transform m_transform{};
  std::unique_ptr<ngf::Texture> m_texture;
  ngf::PrimitiveType m_primitiveType{ngf::PrimitiveType::Triangles};
  std::array<ngf::Vertex, 4> m_vertices{{{.pos={-1.0f, -1.0f}, .color=ngf::Colors::Red, .texCoords={0.0f, 0.0f}},
                                         {.pos={1.0f, -1.0f}, .color=ngf::Colors::Green, .texCoords={1.0f, 0.0f}},
                                         {.pos={1.0f, 1.0f}, .color=ngf::Colors::Blue, .texCoords={1.0f, 1.0f}},
                                         {.pos={-1.0f, 1.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 1.0f}}
                                        }};
  constexpr static std::array<std::uint16_t, 6> Indices{{0, 1, 2, 0, 2, 3}};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}