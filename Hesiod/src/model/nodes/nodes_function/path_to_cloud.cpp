/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void setup_path_to_cloud_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "path");
  p_node->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");
}

void compute_path_to_cloud_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_in = p_node->get_value_ref<hmap::Path>("path");

  if (p_in)
  {
    hmap::Cloud *p_out = p_node->get_value_ref<hmap::Cloud>("cloud");

    // copy the input heightmap
    *p_out = hmap::Cloud(p_in->points);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod