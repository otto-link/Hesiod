/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewColorize::ViewColorize(std::string id)
    : ViewNode(), hesiod::cnode::Colorize(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("RGB");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("RGB");
}

bool ViewColorize::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  {
    int   n_items = std::min(8, (int)this->cmap_map.size());
    float width = 256.f;
    float height = ImGui::GetStyle().ItemSpacing.y +
                   (float)n_items * ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginListBox("##method", ImVec2(width, height)))
      for (auto &[cname, k] : this->cmap_map)
      {
        bool is_selected = false;
        if (this->cmap_choice == k)
          is_selected = true;

        if (ImGui::Selectable(cname.c_str(), is_selected))
        {
          this->cmap_choice = k;
          this->force_update();
          has_changed = true;
        }
        ImGui::SameLine();
        const char *txt = cname.c_str();
        const float text_width = ImGui::CalcTextSize(txt).x;
        ImGui::Dummy(ImVec2(96.f - text_width, 0));
        ImGui::SameLine();
        hesiod::gui::render_colorbar(hesiod::get_colormap_data(k),
                                     ImVec2(0.5f * width, 16));
      }
    ImGui::EndListBox();
  }

  ImGui::Checkbox("reverse", &this->reverse);
  has_changed |= this->trigger_update_after_edit();

  ImGui::Checkbox("clamp", &this->clamp);
  has_changed |= this->trigger_update_after_edit();

  if (this->clamp)
    if (hesiod::gui::slider_vmin_vmax(this->vmin, this->vmax))
    {
      this->force_update();
      has_changed = true;
    }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewColorize::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("cmap_choice", this->cmap_choice));
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("clamp", this->clamp));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

void ViewColorize::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("cmap_choice", this->cmap_choice));
  ar(cereal::make_nvp("reverse", this->reverse));
  ar(cereal::make_nvp("clamp", this->clamp));
  ar(cereal::make_nvp("vmin", this->vmin));
  ar(cereal::make_nvp("vmax", this->vmax));
}

} // namespace hesiod::vnode
