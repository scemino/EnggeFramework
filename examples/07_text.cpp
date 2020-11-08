#include <ngf/Application.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Text.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/System/Util.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <memory>
#include <array>
#include <codecvt>
#include <locale>

namespace {
std::string toUtf8(const std::wstring &text) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(text);
}
}

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="07 - Text", .size={640, 480}});

    m_texture = std::make_unique<ngf::Texture>("./assets/background.jpg");
    m_textureCharacter = std::make_unique<ngf::Texture>("./assets/characters.png");

    m_font = std::make_unique<ngf::FntFont>();
    m_font->loadFromFile("./assets/bmfont_test.fnt");

    m_text = std::make_unique<ngf::Text>();
    m_text->setFont(*m_font);
    m_text->setWideString(L"#FF0000Welcome#0000FF to Engge Framework");
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::LightBlue);
    ngf::RenderStates s;
    s.texture = m_texture.get();
    target.draw(ngf::PrimitiveType::TriangleFan, m_vertices, s);
    m_text->draw(target);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    auto text = m_text->getString();
    if (ImGui::InputText("Text", &text)) {
      m_text->setString(text);
    }
    auto maxWidth = m_text->getMaxWidth();
    if (ImGui::InputFloat("Max width", &maxWidth)) {
      m_text->setMaxWidth(maxWidth);
    }
    auto color = m_text->getColor();
    if (ngf::ImGui::ColorEdit4("Color", &color)) {
      m_text->setColor(color);
    }
    ImGui::End();
  }

private:
  std::unique_ptr<ngf::FntFont> m_font;
  std::unique_ptr<ngf::Text> m_text;
  std::unique_ptr<ngf::Texture> m_texture;
  std::unique_ptr<ngf::Texture> m_textureCharacter;
  std::array<ngf::Vertex, 4> m_vertices{{{.pos={0.0f, 480.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 1.0f}},
                                         {.pos={640.0f, 480.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 1.0f}},
                                         {.pos={640.0f, 0.0f}, .color=ngf::Colors::White, .texCoords={1.0f, 0.0f}},
                                         {.pos={0.0f, 0.0f}, .color=ngf::Colors::White, .texCoords={0.0f, 0.0f}}
                                        }};
  int m_animIndex{0};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}