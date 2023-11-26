/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewEqualize::ViewEqualize(std::string id)
    : hesiod::cnode::ControlNode(id), ViewNode(id), hesiod::cnode::Equalize(id)
{
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

bool ViewEqualize::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

} // namespace hesiod::vnode
