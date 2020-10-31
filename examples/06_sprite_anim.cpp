#include <ngf/Application.h>
#include <ngf/Graphics/Animation.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <Graphics/AnimatedSprite.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="06 - Sprite animation", .size={640, 480}});

    m_texture = std::make_unique<ngf::Texture>("./assets/background.jpg");
    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");

    // define animations
    m_animWalk = std::make_unique<ngf::Animation>();
    m_animWalk->addFrames(*m_textureCharacter, {32, 32}, {0, 64}, {4, 1}, 4);
    m_animJump = std::make_unique<ngf::Animation>();
    m_animJump->addFrames(*m_textureCharacter, {32, 32}, {128, 64}, {3, 1}, 3);
    m_animJump->setFps(4.f);

    // create animated sprite
    m_sprite = std::make_unique<ngf::AnimatedSprite>();
    m_sprite->setAnimation(m_animWalk.get());
    m_sprite->getSprite().getTransform().setScale({4.f,4.f});
    m_sprite->getSprite().setAnchor(ngf::Anchor::Center);
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_sprite->update(elapsed);
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::LightBlue);
    ngf::RenderStates s;
    s.texture = m_texture.get();
    target.draw(m_primitiveType,
                m_vertices.data(),
                m_vertices.size(),
                Indices.data(),
                Indices.size(),
                s);
    m_sprite->draw(target);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    const char *items = {"Walk\0Jump\0\0"};
    if (ImGui::Combo("Anim", &m_animIndex, items)) {
      if (m_animIndex == 0) {
        m_sprite->setAnimation(m_animWalk.get());
      } else {
        m_sprite->setAnimation(m_animJump.get());
      }
    }

    auto &trsf = m_sprite->getSprite().getTransform();
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
  std::unique_ptr<ngf::AnimatedSprite> m_sprite;
  std::unique_ptr<ngf::Animation> m_animWalk;
  std::unique_ptr<ngf::Animation> m_animJump;
  std::unique_ptr<ngf::Texture> m_texture;
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  ngf::PrimitiveType m_primitiveType{ngf::PrimitiveType::Triangles};
  std::array<ngf::Vertex, 4> m_vertices{{{.pos={-320.0f, -240.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 0.0f}},
                                         {.pos={320.0f, -240.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 0.0f}},
                                         {.pos={320.0f, 240.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 1.0f}},
                                         {.pos={-320.0f, 240.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 1.0f}}
                                        }};
  constexpr static std::array<std::uint16_t, 6> Indices{{0, 1, 2, 0, 2, 3}};
  int m_animIndex{0};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}