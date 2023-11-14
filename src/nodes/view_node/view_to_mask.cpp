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

ViewToMask::ViewToMask(std::string id) : ViewNode(), hesiod::cnode::ToMask(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("input");
  this->set_view3d_color_port_id("output");
}

bool ViewToMask::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  has_changed |= render_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node());

  has_changed |= this->render_settings_footer();

  return has_changed;
}

void ViewToMask::serialize_save(cereal::JSONOutputArchive &ar)
{
  serialize_save_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node(),
      ar);
}

void ViewToMask::serialize_load(cereal::JSONInputArchive &ar)
{
  serialize_load_settings_mask(
      (hesiod::cnode::Mask *)this->get_p_control_node(),
      ar);
}

} // namespace hesiod::vnode
