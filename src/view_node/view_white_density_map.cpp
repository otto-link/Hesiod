/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewWhiteDensityMap::ViewWhiteDensityMap(
    hesiod::cnode::WhiteDensityMap *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
  this->seed = p_control_node->get_seed();
  this->set_preview_port_id("output");
}

bool ViewWhiteDensityMap::render_settings()
{
  bool has_changed = false;

  has_changed |= this->render_settings_header();

  ImGui::AlignTextToFramePadding();
  if (ImGui::Button("Re-seed"))
  {
    // TODO to do in the node itself
    this->seed = (int)time(NULL);
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  if (ImGui::DragInt("seed", &this->seed))
  {
    this->p_control_node->set_seed((uint)this->seed);
    has_changed = true;
  }

  has_changed |= this->render_settings_footer();

  return has_changed;
}

} // namespace hesiod::vnode
