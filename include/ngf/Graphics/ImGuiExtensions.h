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
  return ::ImGui::ColorEdit4(label, color != nullptr ? ngf::value_ptr(*color) : nullptr, flags);
}

/// Displays a RGBA color picker.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \param ref_col Reference color.
/// \return true if the color has been changed.
bool ColorPicker4(const char *label, Color *color, ImGuiColorEditFlags flags = 0, const Color *ref_col = nullptr) {
  return ::ImGui::ColorPicker4(label,
                               color != nullptr ? ngf::value_ptr(*color) : nullptr,
                               flags,
                               ref_col != nullptr ? ngf::value_ptr(*ref_col) : nullptr);
}

bool ColorButton(const char *desc_id, const Color &col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0)) {
  return ::ImGui::ColorButton(desc_id, ImVec4(col.r, col.g, col.b, col.a), flags, size);
}
}