#pragma once
#include <imgui.h>
#include <ngf/Graphics/Color.h>

namespace ngf::ImGui {
/// Displays a RGB color editor.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \return true if the color has been changed.
bool ColorEdit3(const char *label, Color *color, ImGuiColorEditFlags flags = 0) {
  return ::ImGui::ColorEdit3(label, ngf::value_ptr(*color), flags);
}

/// Displays a RGBA color editor.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \return true if the color has been changed.
bool ColorEdit4(const char *label, Color *color, ImGuiColorEditFlags flags = 0) {
  return ::ImGui::ColorEdit4(label, ngf::value_ptr(*color), flags);
}
}