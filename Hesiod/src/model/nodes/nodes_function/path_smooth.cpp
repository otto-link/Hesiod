/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_smooth_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  p_node->add_attr<IntAttribute>("navg", 1, 1, 10, "navg");
  p_node->add_attr<FloatAttribute>("averaging_intensity",
                                   1.f,
                                   0.f,
                                   1.f,
                                   "averaging_intensity");
  p_node->add_attr<FloatAttribute>("inertia", 0.f, 0.f, 1.f, "inertia");

  // attribute(s) order
  p_node->set_attr_ordered_key({"navg", "averaging_intensity", "inertia"});
}

void compute_path_smooth_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_in = p_node->get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = p_node->get_value_ref<hmap::Path>("output");

    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->smooth(GET("navg", IntAttribute),
                    GET("averaging_intensity", FloatAttribute),
                    GET("inertia", FloatAttribute));
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod