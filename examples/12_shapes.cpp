#include <ngf/Application.h>
#include <ngf/Graphics/CircleShape.h>
#include <ngf/Graphics/ConvexShape.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/RectangleShape.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shape.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <memory>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="12 - Shapes", .size={640, 480}});
    updateShape();
  }

  void onRender(ngf::RenderTarget &target) override {
    target.setView(ngf::View({0.f, 0.f}, {640.f, 480.f}));
    target.clear(ngf::Colors::LightBlue);
    m_shape->draw(target, {});
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    const char *items = {"Rectangle\0Circle\0Convex\0\0"};
    if (ImGui::Combo("Shape", &m_primitiveIndex, items)) {
      updateShape();
    }
    if (ngf::ImGui::ColorEdit4("Color", &m_color)) {
      m_shape->setColor(m_color);
    }
    if (ImGui::DragFloat("Outline thickness", &m_outlineThickness)) {
      m_shape->setOutlineThickness(m_outlineThickness);
    }
    if (ngf::ImGui::ColorEdit4("Outline Color", &m_outlineColor)) {
      m_shape->setOutlineColor(m_outlineColor);
    }

    ImGui::End();
  }

  void updateShape() {
    switch (m_primitiveIndex) {
    case 0:m_shape = std::make_unique<ngf::RectangleShape>(glm::vec2{300.f, 140.f});
      break;
    case 1:m_shape = std::make_unique<ngf::CircleShape>(140.f);
      break;
    case 2:
      std::array<glm::vec2,5> points{glm::vec2{0.f, 0.f},{150.f, 10.f},{120.f, 90.f},{30.f, 100.f},{0.f, 50.f}};
      m_shape = std::make_unique<ngf::ConvexShape>(points);
      break;
    }
    m_shape->setAnchor(ngf::Anchor::Center);
    m_shape->setColor(m_color);
    m_shape->setOutlineColor(m_outlineColor);
    m_shape->setOutlineThickness(m_outlineThickness);
  }

private:
  std::unique_ptr<ngf::Shape> m_shape;
  ngf::Color m_color{ngf::Colors::HotPink};
  ngf::Color m_outlineColor{ngf::Colors::DarkSlateBlue};
  float m_outlineThickness{8.f};
  int m_primitiveIndex{0};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}