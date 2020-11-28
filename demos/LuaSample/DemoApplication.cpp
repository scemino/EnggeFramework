#define SOL_ALL_SAFETIES_ON 1
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Graphics/DefaultFont.h>
#include <imgui.h>
#include <cstring>
#include "DemoApplication.h"
#include "Graphics.h"

struct Lua {
  static void init() {}
  static void draw() {}
  static void update(float) {}
};

namespace {
std::tuple<int, std::string> parseError(std::string_view error) {
  std::string s(error);
  int i = 0;
  auto pch = strtok(s.data(), ":");
  while (pch != nullptr && i++ < 3) {
    pch = strtok(nullptr, ":");
  }
  auto line = std::strtol(pch, nullptr, 10);
  auto msg = strtok(nullptr, ":");
  return {line, msg};
}
}

DemoApplication::DemoApplication() { m_app = this; }

DemoApplication *DemoApplication::getApp() {
  return m_app;
}

std::shared_ptr<ngf::Font> DemoApplication::getDefaultFont() { return m_font; }

void DemoApplication::onInit() {
  m_window.init({"LuaSample", {640, 480}});

  m_font = std::make_shared<ngf::DefaultFont>("assets/PressStart2P-vaV7.ttf");

  auto lang = TextEditor::LanguageDefinition::Lua();
  m_editor.SetLanguageDefinition(lang);

  lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
  auto luaType = lua.new_usertype<Lua>("lua");

  registerGraphics();

  m_editor.SetText(
      R"(img=null
function lua.init()
  img = image("assets/background.jpg")
end

e=0
r=100
c=1
function lua.update(ms)
e=e+ms
c=0.5+0.5*math.sin(e/200)
r=100+20*math.sin(e/200)
end

function lua.draw()
graphics.clear(c,0.8,0.8,1.0)
graphics.setColor(c,c,c,1)
graphics.setImage(img)
graphics.drawCircle(-r,-r,r)
graphics.setColor(1,1,1,1)
graphics.print("Hello world",0,0)
end)");
  lua.script(m_editor.GetText());
}

void DemoApplication::onUpdate(const ngf::TimeSpan &elapsed) {
  auto ms = elapsed.getTotalMilliseconds();
  try {
    auto tgt = getRenderTarget();
    if (tgt)
      lua["lua"]["update"](ms);
  } catch (const sol::error &error) {
    auto[line, msg] = parseError(error.what());
    TextEditor::ErrorMarkers markers;
    markers.emplace(line, msg);
    m_editor.SetErrorMarkers(markers);
  }
}

void DemoApplication::onRender(ngf::RenderTarget &target) {
  target.setView(ngf::View({0.f, 0.f}, {640.f, 480.f}));
  target.clear(ngf::Colors::LightBlue);
  try {
    auto tgt = getRenderTarget();
    if (tgt)
      lua.script("lua.draw()");
  } catch (const sol::error &error) {
    auto[line, msg] = parseError(error.what());
    TextEditor::ErrorMarkers markers;
    markers.emplace(line, msg);
    m_editor.SetErrorMarkers(markers);
  }
  Application::onRender(target);
}

void DemoApplication::onImGuiRender() {
  ImGui::Begin("Tools");
  ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  m_editor.Render("TextEditor");
  if (m_editor.IsTextChanged()) {
    try {
      lua.script(m_editor.GetText());
      lua.script("lua.init()");
      m_editor.SetErrorMarkers(TextEditor::ErrorMarkers{});
    } catch (const sol::error &error) {
      auto[line, msg] = parseError(error.what());
      TextEditor::ErrorMarkers markers;
      markers.emplace(line, msg);
      m_editor.SetErrorMarkers(markers);
    }
  }
  ImGui::End();
}

void DemoApplication::registerGraphics() {
  lua.new_usertype<Image>("image",
                          sol::call_constructor,
                          sol::factories([](const std::string &d) { return std::make_shared<Image>(d); }));

  lua.new_usertype<Graphics>("graphics",
                             "drawCircle", &Graphics::drawCircle,
                             "clear", sol::overload(&Graphics::clearRgba, &Graphics::clear),
                             "print", &Graphics::print,
                             "setImage", &Graphics::setImage,
                             "setColor", &Graphics::setColor);
}

DemoApplication *DemoApplication::m_app{nullptr};
