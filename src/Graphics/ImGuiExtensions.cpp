#include <ngf/Graphics/ImGuiExtensions.h>
#include <glm/gtc/type_ptr.hpp>

namespace ngf::ImGui {
bool ColorEdit3(const char *label, Color *color, ImGuiColorEditFlags flags) {
  return ::ImGui::ColorEdit3(label, ngf::value_ptr(*color), flags);
}

bool ColorEdit4(const char *label, Color *color, ImGuiColorEditFlags flags) {
  return ::ImGui::ColorEdit4(label, color != nullptr ? ngf::value_ptr(*color) : nullptr, flags);
}

bool ColorPicker4(const char *label, Color *color, ImGuiColorEditFlags flags, const Color *ref_col) {
  return ::ImGui::ColorPicker4(label,
                               color != nullptr ? ngf::value_ptr(*color) : nullptr,
                               flags,
                               ref_col != nullptr ? ngf::value_ptr(*ref_col) : nullptr);
}

bool ColorButton(const char *desc_id, const Color &col, ImGuiColorEditFlags flags, ImVec2 size) {
  return ::ImGui::ColorButton(desc_id, ::ImVec4(col.r, col.g, col.b, col.a), flags, size);
}

bool InputInt2(const char *label, glm::ivec2 *v, ImGuiInputTextFlags flags) {
  return ::ImGui::InputInt2(label, glm::value_ptr(*v), flags);
}

bool InputInt4(const char *label, ngf::irect *v, ImGuiInputTextFlags flags) {
  return ::ImGui::InputInt4(label, &v->min.x, flags);
}

bool InputFloat2(const char *label, glm::vec2 *v, const char *format, ImGuiInputTextFlags flags) {
  return ::ImGui::InputFloat2(label, glm::value_ptr(*v), format, flags);
}

::ImVec4 ImVec4(const ngf::Color &color) {
  return ::ImVec4(color.r, color.g, color.b, color.a);
}

}