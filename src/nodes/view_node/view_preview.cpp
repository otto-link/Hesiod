/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewPreview::ViewPreview(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Preview(id)
{
  this->set_preview_port_id("input");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("color mask");
}

bool ViewPreview::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  // nothing here

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
