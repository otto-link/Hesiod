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

ViewSelectEq::ViewSelectEq(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::SelectEq(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("output");
}

bool ViewSelectEq::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::DragFloat("value", &this->value, 0.001f, -FLT_MAX, +FLT_MAX);
  has_changed |= this->trigger_update_after_edit();

  // unique values
  ImGui::Checkbox("Use input unique values", &this->use_input_unique_values);

  if (this->use_input_unique_values)
  {
    if (ImGui::Button("Update unique values") && this->get_p_data("input"))
    {
      hmap::HeightMap *p_input_hmap = static_cast<hmap::HeightMap *>(
          (void *)this->get_p_data("input"));
      this->input_unique_values = p_input_hmap->unique_values();
    }

    if (this->input_unique_values.size() > 0)
    {
      std::string preview_value = std::to_string(
          this->input_unique_values[this->selected_idx]);

      if (ImGui::BeginCombo("Values", preview_value.c_str()))
      {
        int kmax = std::min(16, (int)this->input_unique_values.size());
        for (int k = 0; k < kmax; k++)
        {
          const bool is_selected = (this->selected_idx == k);
          if (ImGui::Selectable(
                  std::to_string(this->input_unique_values[k]).c_str(),
                  is_selected))
          {
            this->selected_idx = k;
            this->value = this->input_unique_values[k];
            has_changed = true;
            this->force_update();
          }

          // Set the initial focus when opening the combo (scrolling +
          // keyboard navigation focus)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
    }
  }

  ImGui::Separator();
  has_changed |= render_settings_mask(this->get_ref<hesiod::cnode::Mask>());

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
