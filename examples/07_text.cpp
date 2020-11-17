#include <ngf/Application.h>
#include <ngf/Graphics/DefaultFont.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Text.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"07 - Text", {640, 480}});

    m_font.loadFromFile("./assets/PressStart2P-vaV7.ttf");
    m_text.setFont(m_font);
    m_text.setWideString(L"Welcome to Engge Framework");
    m_text.setAnchor(ngf::Anchor::Center);
  }

  void onRender(ngf::RenderTarget &target) override {
    target.setView(ngf::View(ngf::frect::fromCenterSize({0, 0}, {640, 480})));
    target.clear(ngf::Colors::LightBlue);
    m_text.draw(target);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    auto text = m_text.getString();
    if (ImGui::InputText("Text", &text)) {
      m_text.setString(text);
      m_text.setAnchor(ngf::Anchor::Center);
    }
    auto maxWidth = m_text.getMaxWidth();
    if (ImGui::DragFloat("Max width", &maxWidth)) {
      m_text.setMaxWidth(maxWidth);
      m_text.setAnchor(ngf::Anchor::Center);
    }
    const char *alignments = "None\0Left\0Right\0Center\0Justify\0\0";
    auto align = static_cast<int>(m_text.getAlignment());
    if (ImGui::Combo("Alignment", &align, alignments)) {
      m_text.setAlignment(static_cast<ngf::Alignment>(align));
      m_text.setAnchor(ngf::Anchor::Center);
    }
    auto color = m_text.getColor();
    if (ngf::ImGui::ColorEdit4("Color", &color)) {
      m_text.setColor(color);
    }
    auto outlineThickness = m_text.getOutlineThickness();
    if (ImGui::DragFloat("Outline Thickness", &outlineThickness)) {
      m_text.setOutlineThickness(outlineThickness);
    }
    auto outlineColor = m_text.getOutlineColor();
    if (ngf::ImGui::ColorEdit4("Outline Color", &outlineColor)) {
      m_text.setOutlineColor(outlineColor);
    }
    auto characterSize = (int) m_text.getCharacterSize();
    if (ImGui::DragInt("Character size", &characterSize)) {
      m_text.setCharacterSize(characterSize);
      m_text.setAnchor(ngf::Anchor::Center);
    }
    auto lineSpacing = m_text.getLineSpacing();
    if (ImGui::DragFloat("Line spacing", &lineSpacing)) {
      m_text.setLineSpacing(lineSpacing);
      m_text.setAnchor(ngf::Anchor::Center);
    }
    auto letterSpacing = m_text.getLetterSpacing();
    if (ImGui::DragFloat("Letter spacing", &letterSpacing)) {
      m_text.setLetterSpacing(letterSpacing);
      m_text.setAnchor(ngf::Anchor::Center);
    }
    ImGui::End();
  }

private:
  ngf::DefaultFont m_font;
  ngf::Text m_text;
  int m_animIndex{0};
};

int main(int argc, char* argv[]) {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}