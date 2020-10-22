#pragma once
#include <imgui.h>
#include <ngf/Graphics/Color.h>

namespace ngf::ImGui {
bool ColorEdit3(const char *label, Color *color, ImGuiColorEditFlags flags = 0) {
  return ::ImGui::ColorEdit3(label, ngf::value_ptr(*color), flags);
}

bool ColorEdit4(const char *label, Color *color, ImGuiColorEditFlags flags = 0) {
  return ::ImGui::ColorEdit4(label, ngf::value_ptr(*color), flags);
}
}