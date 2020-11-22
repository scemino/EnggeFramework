#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/RenderTexture.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Math/Transform.h>
#include <imgui.h>
#include <memory>
#include <ngf/Graphics/Tween.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"12 - Render Texture", {640, 480}});

    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");

    auto rect = ngf::irect::fromMinMax({6, 73}, {23, 95});
    m_sprite = std::make_unique<ngf::Sprite>(*m_textureCharacter, rect);
    m_sprite->setAnchor(ngf::Anchor::Center);
    m_sprite->getTransform().setScale({4, 4});
    m_sprite->setColor(ngf::Colors::Yellow);

    auto size = m_window.getFramebufferSize();
    m_renderTexture = std::make_unique<ngf::RenderTexture>(size);
  }

  void onEvent(ngf::Event &event) override {
    switch (event.type) {
    case ngf::EventType::KeyPressed:
      if(event.key.keycode == ngf::Keycode::Q){
        quit();
      }
      break;
    default:break;
    }
  }

  void onRender(ngf::RenderTarget &target) override {
    m_renderTexture->activate();
    m_renderTexture->setView(ngf::View(ngf::frect::fromCenterSize({0, 0}, {640, 480})));
    m_renderTexture->clear(ngf::Colors::Gray);
    m_sprite->draw(*m_renderTexture);
    m_renderTexture->display();

    target.activate();
    target.setView(ngf::View(ngf::frect::fromCenterSize({0, 0}, {640, 480})));
    target.clear(ngf::Colors::LightBlue);
    ngf::Sprite sprite(m_renderTexture->getTexture());
    sprite.setAnchor(ngf::Anchor::Center);
    sprite.draw(target);
    m_sprite->draw(target);

    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    auto &trsf = m_sprite->getTransform();
    auto origin = trsf.getOrigin();
    if (ImGui::DragFloat2("Origin", glm::value_ptr(origin), 1.f)) {
      trsf.setOrigin(origin);
    }
    auto pos = trsf.getPosition();
    if (ImGui::DragFloat2("Position", glm::value_ptr(pos), 1.f)) {
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
    ImGui::End();
  }

private:
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  std::unique_ptr<ngf::Sprite> m_sprite;
  std::unique_ptr<ngf::RenderTexture> m_renderTexture;
  glm::vec2 m_worldPos{};
  ngf::Color m_color = {ngf::Colors::White, 0.5f};
  std::array<ngf::Vertex, 4> m_vertices{{{{-320.0f, -240.0f}, ngf::Colors::Red},
                                         {{320.0f, -240.0f}, m_color},
                                         {{320.0f, 240.0f}, m_color},
                                         {{-320.0f, 240.0f}, m_color}
                                        }};
};

int main(int argc, char* argv[]) {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}