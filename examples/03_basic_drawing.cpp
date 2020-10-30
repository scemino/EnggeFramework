#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <memory>
#include <array>
#include <sstream>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="03 - Basic Drawing", .size={640, 480}});
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::Lightblue);
    target.draw(m_primitiveType,
                m_vertices.data(),
                m_vertices.size(),
                Indices.data(),
                Indices.size());
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    const char *items = {"Points\0LineStrip\0LineLoop\0Lines\0TriangleStrip\0TriangleFan\0Triangles\0\0"};
    auto primitiveIndex = static_cast<int>(m_primitiveType);
    if (ImGui::Combo("Primitive type", &primitiveIndex, items)) {
      m_primitiveType = static_cast<ngf::PrimitiveType>(primitiveIndex);
    }
    for (size_t i = 0; i < m_vertices.size(); ++i) {
      std::ostringstream o;
      o << "Vertex #" << (i + 1);
      if (ImGui::TreeNode(o.str().c_str())) {
        ImGui::DragFloat2("Position", glm::value_ptr(m_vertices[i].pos), 1.f, -320.0f, 320.0f);
        ngf::ImGui::ColorEdit4("Color", &m_vertices[i].color);
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

private:
  ngf::PrimitiveType m_primitiveType{ngf::PrimitiveType::Triangles};
  std::array<ngf::Vertex, 4> m_vertices{{{.pos={-320.0f, -240.0f}, .color=ngf::Colors::Red},
                                         {.pos={320.0f, -240.0f}, .color=ngf::Colors::Green},
                                         {.pos={320.0f, 240.0f}, .color=ngf::Colors::Blue},
                                         {.pos={-320.0f, 240.0f}, .color=ngf::Colors::White}
                                        }};
  constexpr static std::array<std::uint16_t, 6> Indices{{0, 1, 2, 0, 2, 3}};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}