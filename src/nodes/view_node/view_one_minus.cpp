/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewOneMinus::ViewOneMinus(std::string id)
    : ViewNode(), hesiod::cnode::OneMinus(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewOneMinus::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  if (hesiod::gui::slider_vmin_vmax(vmin, vmax))
  {
    this->force_update();
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewOneMinus::serialize_save(cereal::JSONOutputArchive &)
{
  // empty
}

void ViewOneMinus::serialize_load(cereal::JSONInputArchive &)
{
  // empty
}

} // namespace hesiod::vnode
