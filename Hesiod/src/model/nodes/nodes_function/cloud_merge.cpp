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

void setup_cloud_merge_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud1");
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud2");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");
}

void compute_cloud_merge_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_in1 = p_node->get_value_ref<hmap::Cloud>("cloud1");
  hmap::Cloud *p_in2 = p_node->get_value_ref<hmap::Cloud>("cloud2");

  if (p_in1 && p_in2)
  {
    hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

    // convert the input
    *p_out = hmap::merge_cloud(*p_in1, *p_in2);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
