/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewBlend::ViewBlend(std::string id) : ViewNode(), hesiod::cnode::Blend(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewBlend::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  float height = ImGui::GetStyle().ItemSpacing.y +
                 (float)this->blending_method_map.size() *
                     ImGui::GetTextLineHeightWithSpacing();

  ImGui::TextUnformatted("Method");
  if (ImGui::BeginListBox("##method", ImVec2(-FLT_MIN, height)))
    for (auto &[cname, k] : this->blending_method_map)
    {
      bool is_selected = false;
      if (this->method == k)
        is_selected = true;

      if (ImGui::Selectable(cname.c_str(), is_selected))
      {
        this->method = k;
        this->force_update();
        has_changed = true;
      }
    }
  ImGui::EndListBox();

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
