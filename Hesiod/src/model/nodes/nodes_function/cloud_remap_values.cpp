/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_remap_values_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "output");

  // attribute(s)
  ADD_ATTR(RangeAttribute, "remap");
}

void compute_cloud_remap_values_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Cloud *p_in = p_node->get_value_ref<hmap::Cloud>("input");

  if (p_in)
  {
    hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("output");

    // copy and remap the input
    *p_out = *p_in;
    p_out->remap_values(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
