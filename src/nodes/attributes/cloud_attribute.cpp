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

hmap::Cloud CloudAttribute::get()
{
  return value;
}

bool CloudAttribute::render_settings(std::string label)
{
  bool has_changed = false;
  ImGui::TextUnformatted(label.c_str());
  has_changed |= hesiod::gui::canvas_cloud_editor(this->value);
  return has_changed;
}

} // namespace hesiod
