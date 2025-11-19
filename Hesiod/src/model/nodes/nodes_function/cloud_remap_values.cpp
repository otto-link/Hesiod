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

void setup_cloud_remap_values_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "input");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "output");

  // attribute(s)
  node.add_attr<RangeAttribute>("remap", "remap");
}

void compute_cloud_remap_values_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_in = node.get_value_ref<hmap::Cloud>("input");

  if (p_in)
  {
    hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("output");

    // copy and remap the input
    *p_out = *p_in;
    p_out->remap_values(node.get_attr<RangeAttribute>("remap")[0],
                        node.get_attr<RangeAttribute>("remap")[1]);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
