/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewRecurveS::ViewRecurveS(std::string id)
    : ViewNode(), hesiod::cnode::RecurveS(id)
{
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
  this->set_view3d_elevation_port_id("output");
}

void ViewRecurveS::serialize_save(cereal::JSONOutputArchive &)
{
  // nothing to do
}

void ViewRecurveS::serialize_load(cereal::JSONInputArchive &)
{
  // nothing to do
}

} // namespace hesiod::vnode
