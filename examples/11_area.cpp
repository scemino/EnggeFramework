#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Math/Math.h>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <imgui.h>
#include <memory>
#include <ngf/Graphics/Tween.h>
#include <ngf/Graphics/InputArea.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="11 - Button", .size={640, 480}});

    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");

    auto rect = ngf::irect::fromMinMax({6, 73}, {23, 95});
    m_sprite = std::make_unique<ngf::Sprite>(*m_textureCharacter, rect);
    m_sprite->setAnchor(ngf::Anchor::Center);

    m_area = createArea(*m_sprite, [this](auto type, glm::vec2 pos) {
      switch (type) {
      case ngf::ButtonEventType::Enter: {
        m_tweenRotation = ngf::Tweening::make(-10.f, 10.f)
            .with(ngf::Easing::linear)
            .setDuration(ngf::TimeSpan::seconds(0.2f))
            .repeat(3, ngf::RepeatBehavior::TwoWay)
            .onValueChanged([this](auto value) { m_sprite->getTransform().setRotation(value); });
      }
        break;
      case ngf::ButtonEventType::Leave:m_sprite->setColor(ngf::Colors::White);
        break;
      case ngf::ButtonEventType::Down:m_sprite->setColor(ngf::Colors::Blue);
        break;
      case ngf::ButtonEventType::Up:m_sprite->setColor(ngf::Colors::Red);
        break;
      case ngf::ButtonEventType::Drag:m_sprite->getTransform().setPosition(pos);
        break;
      case ngf::ButtonEventType::Drop:m_sprite->setColor(ngf::Colors::Green);
        break;
      default:break;
      }
    });
  }

  void onEvent(ngf::Event &event) override {
    manageAreaEvent(*getRenderTarget(), m_area, event);
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_tweenRotation.update(elapsed);
  }

  void onRender(ngf::RenderTarget &target) override {
    auto rect = m_sprite->getLocalBounds();
    std::vector<ngf::Vertex> vertices(4);
    vertices[0].pos = rect.min;
    vertices[0].color = ngf::Colors::Green;
    vertices[1].pos = {rect.min.x, rect.max.y};
    vertices[1].color = ngf::Colors::Green;
    vertices[2].pos = {rect.max.x, rect.max.y};
    vertices[2].color = ngf::Colors::Green;
    vertices[3].pos = {rect.max.x, rect.min.y};
    vertices[3].color = ngf::Colors::Green;

    target.clear(ngf::Colors::LightBlue);
    m_sprite->draw(target);

    ngf::RenderStates s;
    s.transform = m_sprite->getTransform().getTransform();
    target.draw(ngf::PrimitiveType::LineLoop, vertices, s);

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
    ImGui::End();
  }

private:
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  std::unique_ptr<ngf::Sprite> m_sprite;
  ngf::Tween<float> m_tweenRotation;
  ngf::InputArea<ngf::Sprite> m_area;
  glm::vec2 m_worldPos{};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}