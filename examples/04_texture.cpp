#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/Vertex.h>
#include <Math/Transform.h>
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

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::LightBlue);
    ngf::RenderStates s;
    s.transform *= m_transform.getTransform();
    s.texture = m_texture.get();
    target.draw(ngf::PrimitiveType::TriangleFan, m_vertices, s);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
        ImGui::DragFloat2("Position", glm::value_ptr(m_vertices[i].pos), 1.f, -320.0f, 320.0f);
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
  std::array<ngf::Vertex, 4> m_vertices{{{.pos={0.0f, 480.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 1.0f}},
                                         {.pos={640.0f, 480.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 1.0f}},
                                         {.pos={640.0f, 0.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 0.0f}},
                                         {.pos={0.0f, 0.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 0.0f}}
                                        }};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}