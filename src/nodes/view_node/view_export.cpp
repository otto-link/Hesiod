/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewExport::ViewExport(std::string id) : ViewNode(), hesiod::cnode::Export(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("input");
}

void ViewExport::render_node_specific_content()
{
  if (ImGui::Button("export!"))
    this->write_file();
}

bool ViewExport::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  ImGui::Checkbox("Auto export", &this->auto_export);

  ImGui::TextUnformatted("Format");
  hesiod::gui::listbox_map_enum(this->format_map, this->export_format);

  if (ImGui::Button("export"))
    this->write_file();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewExport::serialize_save(cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("auto_export", this->auto_export));
  ar(cereal::make_nvp("export_format", this->export_format));
  ar(cereal::make_nvp("fname", this->fname));
}

void ViewExport::serialize_load(cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("auto_export", this->auto_export));
  ar(cereal::make_nvp("export_format", this->export_format));
  ar(cereal::make_nvp("fname", this->fname));
}

} // namespace hesiod::vnode
