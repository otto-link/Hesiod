/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewExportRGB::ViewExportRGB(std::string id)
    : ViewNode(), hesiod::cnode::ExportRGB(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("RGB");
}

void ViewExportRGB::render_node_specific_content()
{
  if (ImGui::Button("export!"))
    this->write_file();
}

bool ViewExportRGB::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("Auto export", &this->auto_export);

  {
    if (ImGui::Button(("File: " + this->fname).c_str()))
      ImGuiFileDialog::Instance()->OpenDialog("DialogId",
                                              "Choose File",
                                              ".png,.raw,.bin",
                                              ".");

    if (ImGuiFileDialog::Instance()->Display("DialogId"))
    {
      if (ImGuiFileDialog::Instance()->IsOk())
        this->fname = ImGuiFileDialog::Instance()->GetFilePathName(
            IGFD_ResultMode_AddIfNoFileExt);
      ImGuiFileDialog::Instance()->Close();
    }
  }

  if (ImGui::Button("export"))
    this->write_file();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewExportRGB::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("auto_export", this->auto_export));
  ar(cereal::make_nvp("fname", this->fname));
}

void ViewExportRGB::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("auto_export", this->auto_export));
  ar(cereal::make_nvp("fname", this->fname));
}

} // namespace hesiod::vnode
