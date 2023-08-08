/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewGammaCorrection::ViewGammaCorrection(
    hesiod::cnode::GammaCorrection *p_control_node)
    : ViewNode((gnode::Node *)p_control_node), p_control_node(p_control_node)
{
}

bool ViewGammaCorrection::render_settings()
{
  bool has_changed = false;

  LOG_DEBUG("rendering settings...");
  return has_changed;
}

} // namespace hesiod::vnode
