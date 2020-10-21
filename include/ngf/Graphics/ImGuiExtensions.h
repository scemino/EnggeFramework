#pragma once
#include <imgui.h>
#include <ngf/Graphics/Color.h>

namespace ngf::ImGui {
bool ColorEdit3(const char *label, Color *col, ImGuiColorEditFlags flags = 0) {
  float color[]
      {static_cast<float>(col->r) / 255.f, static_cast<float>(col->g) / 255.f, static_cast<float>(col->b) / 255.f};
  if (::ImGui::ColorEdit3(label, color, flags)) {
    col->r = static_cast<std::uint8_t>(color[0] * 255.f);
    col->g = static_cast<std::uint8_t>(color[1] * 255.f);
    col->b = static_cast<std::uint8_t>(color[2] * 255.f);
    return true;
  }
  return false;
}

bool ColorEdit4(const char *label, Color *col, ImGuiColorEditFlags flags = 0) {
  float color[]
      {static_cast<float>(col->r) / 255.f, static_cast<float>(col->g) / 255.f, static_cast<float>(col->b) / 255.f,
       static_cast<float>(col->a) / 255.f};
  if (::ImGui::ColorEdit4(label, color, flags)) {
    col->r = static_cast<std::uint8_t>(color[0] * 255.f);
    col->g = static_cast<std::uint8_t>(color[1] * 255.f);
    col->b = static_cast<std::uint8_t>(color[2] * 255.f);
    col->a = static_cast<std::uint8_t>(color[3] * 255.f);
    return true;
  }
  return false;
}
}