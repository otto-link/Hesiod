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

void setup_cloud_to_path_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "path");

  // attribute(s)
  ADD_ATTR(BoolAttribute, "closed", false);
  ADD_ATTR(BoolAttribute, "reorder_nns", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"closed", "reorder_nns"});
}

void compute_cloud_to_path_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Cloud *p_in = p_node->get_value_ref<hmap::Cloud>("cloud");

  if (p_in)
  {
    hmap::Path *p_out = p_node->get_value_ref<hmap::Path>("path");

    // convert the input
    *p_out = hmap::Path(p_in->points);

    p_out->closed = GET("closed", BoolAttribute);

    if (GET("reorder_nns", BoolAttribute))
      p_out->reorder_nns();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
