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
    : ViewNode(), hesiod::cnode::SelectEq(id)
{
  LOG_DEBUG("ViewSelectEq::ViewSelectEq()");
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
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

  if (render_settings_mask(this->smoothing,
                           this->ir_smoothing,
                           this->normalize,
                           this->inverse))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewSelectEq::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("value", this->value));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

void ViewSelectEq::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("value", this->value));

  ar(cereal::make_nvp("normalize", this->normalize));
  ar(cereal::make_nvp("inverse", this->inverse));
  ar(cereal::make_nvp("smoothing", this->smoothing));
  ar(cereal::make_nvp("ir_smoothing", this->ir_smoothing));
}

} // namespace hesiod::vnode
