/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/cmap.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::vnode
{

bool ViewColorize::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  {
    std::map<std::string, int> cmap_map =
        this->attr.at("colormap")->get_ref<MapEnumAttribute>()->get_map();
    int cmap_choice =
        this->attr.at("colormap")->get_ref<MapEnumAttribute>()->get();

    int   n_items = std::min(8, (int)cmap_map.size());
    float width = 256.f;
    float height = ImGui::GetStyle().ItemSpacing.y +
                   (float)n_items * ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginListBox("##method", ImVec2(width, height)))
    {
      for (auto &[cname, k] : cmap_map)
      {
        bool is_selected = false;
        if (cmap_choice == k)
          is_selected = true;

        if (ImGui::Selectable(cname.c_str(), is_selected))
        {
          cmap_choice = k;
          this->attr.at("colormap")->get_ref<MapEnumAttribute>()->set(cname);
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
  }

  has_changed |= hesiod::gui::render_attribute_widget(this->attr.at("reverse"),
                                                      "reverse");
  has_changed |= hesiod::gui::render_attribute_widget(this->attr.at("clamp"),
                                                      "clamp");

  if (has_changed)
    this->force_update();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
