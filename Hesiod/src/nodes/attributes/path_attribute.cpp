/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

hmap::Path PathAttribute::get()
{
  return value;
}

bool PathAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::TextUnformatted(label.c_str());

  ImGui::Checkbox("closed", &this->value.closed);
  has_changed |= ImGui::IsItemDeactivatedAfterEdit();

  ImGui::SameLine();

  if (ImGui::Button("reverse"))
  {
    this->value.reverse();
    has_changed = true;
  }

  has_changed |= hesiod::gui::canvas_path_editor(this->value);
  return has_changed;
}

} // namespace hesiod
