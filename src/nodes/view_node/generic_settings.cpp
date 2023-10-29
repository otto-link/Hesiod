/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

bool render_settings_mask(bool &smoothing,
                          int  &ir_smoothing,
                          bool &normalize,
                          bool &inverse)
{
  bool ret = false;

  if (ImGui::Checkbox("smoothing", &smoothing))
    ret = true;

  if (smoothing)
  {
    ImGui::SliderInt("ir_smoothing", &ir_smoothing, 1, 256);
    if (ImGui::IsItemDeactivatedAfterEdit())
      ret = true;
  }

  if (ImGui::Checkbox("normalize", &normalize))
    ret = true;

  if (ImGui::Checkbox("inverse", &inverse))
    ret = true;

  return ret;
}

} // namespace hesiod::vnode
