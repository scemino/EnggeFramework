#pragma once
#include <imgui.h>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Rect.h>
#include <glm/vec2.hpp>

namespace ngf::ImGui {
/// Displays a RGB color editor.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \return true if the color has been changed.
bool ColorEdit3(const char *label, Color *color, ImGuiColorEditFlags flags = 0);

/// Displays a RGBA color editor.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \return true if the color has been changed.
bool ColorEdit4(const char *label, Color *color, ImGuiColorEditFlags flags = 0);

/// Displays a RGBA color picker.
/// \param label Label to display before the color editor.
/// \param color The color to display/edit.
/// \param flags Flags used to customize the color editor.
/// \param ref_col Reference color.
/// \return true if the color has been changed.
bool ColorPicker4(const char *label, Color *color, ImGuiColorEditFlags flags = 0, const Color *ref_col = nullptr);

bool ColorButton(const char *desc_id, const Color &col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));

bool InputInt2(const char *label, glm::ivec2 *v, ImGuiInputTextFlags flags = 0);

bool InputInt4(const char *label, ngf::irect *v, ImGuiInputTextFlags flags = 0);

bool InputFloat2(const char *label, glm::vec2 *v, const char *format = "%.3f", ImGuiInputTextFlags flags = 0);

::ImVec4 ImVec4(const ngf::Color &color);

}