#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Text.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/IO/Json/JsonParser.h>
#include <imgui.h>
#include <memory>
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
    m_window.init({.title="08 - Json", .size={640, 480}});

    m_sprite = std::make_unique<ngf::Sprite>();
    m_texture = std::make_unique<ngf::Texture>("./assets/tiles.png");
    m_sprite->setTexture(*m_texture);

    m_json = ngf::Json::load("./assets/tiles.json");
    auto tileGrass1 = m_json["frames"]["tileGrass1.png"]["frame"];
    auto rect = toRect(tileGrass1);
    m_sprite->setTextureRect(rect);
    m_sprite->setAnchor(ngf::Anchor::Center);

    for (const auto &frame : m_json["frames"].items()) {
      m_frames.push_back(frame.key());
    }
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::Lightblue);
    ngf::RenderStates s;
    s.texture = m_texture.get();
    m_sprite->draw(target);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::ListBox("Frame", &m_index, items_getter, &m_frames, m_frames.size())) {
      auto frame = toRect(m_json["frames"][m_frames[m_index]]["frame"]);
      m_sprite->setTextureRect(frame);
    }
    ImGui::End();
  }

  static ngf::irect toRect(const ngf::GGPackValue& value){
    auto rect = ngf::irect::fromPositionSize(
        {value["x"].getInt(), value["y"].getInt()},
        {value["w"].getInt(), value["h"].getInt()});
    return rect;
  }

  static bool items_getter(void *ptr, int index, const char **item) {
    auto items = static_cast<std::vector<std::string> *>(ptr);
    if (index >= 0 && index < items->size()) {
      *item = (*items)[index].c_str();
      return true;
    }
    return false;
  }

private:
  std::unique_ptr<ngf::Sprite> m_sprite;
  std::unique_ptr<ngf::Texture> m_texture;
  std::vector<std::string> m_frames;
  int m_index{0};
  ngf::GGPackValue m_json;
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}