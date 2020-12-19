#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/IO/GGPackValue.h>
#include <imgui.h>
#include "AnimEditor.h"

class AnimViewerApplication final : public ngf::Application {
public:
  void load(std::string path) {
    m_path = std::move(path);
  }

private:
  void onInit() override {
    m_window.init({"AnimViewer", {1024, 768}});
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (!m_path.empty()) {
      m_editor.loadAnim(m_path);
    }
  }

  void onEvent(ngf::Event &event) override {
    switch (event.type) {
    case ngf::EventType::DropFile: {
      m_editor.loadAnim(event.drop.file);
    }
      break;
    default:break;
    }
  }

  void onRender(ngf::RenderTarget &target) override {
    m_editor.onRender(target);
    Application::onRender(target);
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_editor.update(elapsed);
  }

  void onImGuiRender() override {
    m_editor.draw();
  }

private:
  std::string m_path;
  AnimEditor m_editor;
};

int main(int argc, char **argv) {
  AnimViewerApplication app{};
  if (argc > 1) {
    app.load(argv[1]);
  }
  app.run();
  return EXIT_SUCCESS;
}