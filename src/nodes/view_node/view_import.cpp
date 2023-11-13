/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewImport::ViewImport(std::string     id,
                       hmap::Vec2<int> shape,
                       hmap::Vec2<int> tiling,
                       float           overlap)
    : ViewNode(), hesiod::cnode::Import(id, shape, tiling, overlap)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

void ViewImport::render_node_specific_content()
{
  if (ImGui::Button("reload!"))
    this->force_update();
}

bool ViewImport::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  {
    if (ImGui::Button(("File: " + this->fname).c_str()))
      ImGuiFileDialog::Instance()->OpenDialog("DialogId",
                                              "Choose File",
                                              ".png",
                                              ".");

    if (ImGuiFileDialog::Instance()->Display("DialogId"))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
        this->fname = ImGuiFileDialog::Instance()->GetFilePathName(
            IGFD_ResultMode_AddIfNoFileExt);
        this->force_update();
      }
      ImGuiFileDialog::Instance()->Close();
    }
  }

  if (ImGui::Button("Reload"))
    this->force_update();

  ImGui::Separator();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewImport::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("fname", this->fname));
}

void ViewImport::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("fname", this->fname));
}

} // namespace hesiod::vnode
