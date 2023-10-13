/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

ViewMedian3x3::ViewMedian3x3(std::string id)
    : ViewNode(), hesiod::cnode::Median3x3(id)
{
  LOG_DEBUG("hash_id: %d", this->hash_id);
  LOG_DEBUG("label: %s", this->label.c_str());
  this->set_p_control_node((gnode::Node *)this);
  this->set_preview_port_id("output");
}

bool ViewMedian3x3::render_settings()
{
  bool has_changed = false;
  has_changed |= this->render_settings_header();

  has_changed |= this->render_settings_footer();
  return has_changed;
}

void ViewMedian3x3::serialize_save(cereal::JSONOutputArchive &)
{
}

void ViewMedian3x3::serialize_load(cereal::JSONInputArchive &)
{
}

} // namespace hesiod::vnode
