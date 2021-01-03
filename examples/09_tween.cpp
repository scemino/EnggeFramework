#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/Tween.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <sstream>
#include <ngf/Graphics/RenderTexture.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"09 - Tween", {640, 480}});

    m_texture = std::make_unique<ngf::Texture>("./assets/background.jpg");
    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");
    m_renderTexture = std::make_unique<ngf::RenderTexture>(glm::ivec2{320,180});

    auto rect = ngf::irect::fromMinMax({6, 73}, {23, 95});
    m_sprite = std::make_unique<ngf::Sprite>(*m_textureCharacter, rect);
    m_sprite->setAnchor(ngf::Anchor::Center);
    m_sprite->getTransform().setScale({2.f, 2.f});

    m_tween = ngf::Tweening::make(glm::vec2{220.0f, 240.f}, {420.0f, 240.f})
        .with(ngf::Easing::quarticEaseOut)
        .setDuration(ngf::TimeSpan::seconds(2))
        .repeat(10, ngf::RepeatBehavior::TwoWay)
        .onValueChanged([this](auto value) { m_sprite->getTransform().setPosition(value); });

    m_tweenRotation = ngf::Tweening::make(-10.f, 10.f)
        .with(ngf::Easing::bounceEaseIn)
        .setDuration(ngf::TimeSpan::seconds(1.5f))
        .repeat(ngf::Times::Infinity, ngf::RepeatBehavior::TwoWay)
        .onValueChanged([this](auto value) { m_sprite->getTransform().setRotation(value); });

    m_tweenColor = ngf::Tweening::make(ngf::Colors::White, ngf::Colors::Red)
        .with(ngf::Easing::backEaseIn)
        .setDuration(ngf::TimeSpan::seconds(1))
        .repeat(ngf::Times::Infinity, ngf::RepeatBehavior::TwoWay)
        .onValueChanged([this](const auto &value) {
          m_sprite->setColor(value);
        });
  }

  void onEvent(ngf::Event &event) override {
    switch (event.type) {
    case ngf::EventType::KeyReleased:
      if (event.key.keycode == ngf::Keycode::S) {
        auto screenshot = ((ngf::RenderWindow*)getRenderTarget())->capture();
        screenshot.saveToFile("screenshot.png");
      }
      break;
    default:break;
    }
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_tween.update(elapsed);
    m_tweenRotation.update(elapsed);
    m_tweenColor.update(elapsed);
  }

  void onRender(ngf::RenderTarget &target) override {
    target.activate();
    renderScene(target);
    Application::onRender(target);
  }

  void renderScene(ngf::RenderTarget &target) {
    target.clear(ngf::Colors::LightBlue);
    ngf::RenderStates s;
    s.texture = m_texture.get();
    target.draw(ngf::PrimitiveType::TriangleFan, m_vertices, s);
    m_sprite->draw(target);
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
  std::unique_ptr<ngf::RenderTexture> m_renderTexture;
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  std::unique_ptr<ngf::Sprite> m_sprite;
  std::array<ngf::Vertex, 4> m_vertices{{{{0.0f, 480.0f}, ngf::Colors::White, {0.0f, 1.0f}},
                                         {{640.0f, 480.0f}, ngf::Colors::White, {1.0f, 1.0f}},
                                         {{640.0f, 0.0f}, ngf::Colors::White, {1.0f, 0.0f}},
                                         {{0.0f, 0.0f}, ngf::Colors::White, {0.0f, 0.0f}}
                                        }};
  ngf::Tween<glm::vec2> m_tween;
  ngf::Tween<float> m_tweenRotation;
  ngf::Tween<ngf::Color> m_tweenColor;
};

int main(int argc, char *argv[]) {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}