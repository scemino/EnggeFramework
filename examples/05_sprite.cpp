#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <sstream>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="05 - Sprite", .size={640, 480}});

    m_texture = std::make_unique<ngf::Texture>("./assets/background.jpg");
    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");

    auto rect = ngf::irect::fromMinMax({6, 73}, {23, 95});
    m_sprite = std::make_unique<ngf::Sprite>(*m_textureCharacter, rect);
    m_sprite->setAnchor(ngf::Anchor::Center);
    m_sprite->getTransform().setPosition({320.f,240.f});
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::LightBlue);
    ngf::RenderStates s;
    s.texture = m_texture.get();
    target.draw(ngf::PrimitiveType::TriangleFan, m_vertices, s);
    m_sprite->draw(target);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    auto &trsf = m_sprite->getTransform();
    auto origin = trsf.getOrigin();
    if (ImGui::DragFloat2("Origin", glm::value_ptr(origin), 0.1f)) {
      trsf.setOrigin(origin);
    }
    auto pos = trsf.getPosition();
    if (ImGui::DragFloat2("Position", glm::value_ptr(pos), 0.1f)) {
      trsf.setPosition(pos);
    }
    auto rotation = trsf.getRotation();
    if (ImGui::DragFloat("Rotation", &rotation, 1.f, -360, 360.0f)) {
      trsf.setRotation(rotation);
    }
    auto scale = trsf.getScale();
    if (ImGui::DragFloat2("Scale", glm::value_ptr(scale), 0.1f, 0.1f, 10.f)) {
      trsf.setScale(scale);
    }
    for (size_t i = 0; i < m_vertices.size(); ++i) {
      std::ostringstream o;
      o << "Vertex #" << (i + 1);
      if (ImGui::TreeNode(o.str().c_str())) {
        ImGui::DragFloat2("Position", glm::value_ptr(m_vertices[i].pos), 0.01f, -320.0f, 320.0f);
        ImGui::DragFloat2("Texture coordinates", glm::value_ptr(m_vertices[i].texCoords), 0.01f, 0.0f, 1.0f);
        ngf::ImGui::ColorEdit4("Color", &m_vertices[i].color);
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

private:
  std::unique_ptr<ngf::Texture> m_texture;
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  std::unique_ptr<ngf::Sprite> m_sprite;
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