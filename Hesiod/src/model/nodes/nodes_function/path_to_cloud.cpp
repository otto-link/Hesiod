/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/cloud.hpp"
#include "highmap/geometry/path.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

namespace hesiod
{

void setup_path_to_cloud_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");
}

void compute_path_to_cloud_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_in = node.get_value_ref<hmap::Path>("path");

  if (p_in)
  {
    hmap::Cloud *p_out = node.get_value_ref<hmap::Cloud>("cloud");

    // copy the input heightmap
    *p_out = hmap::Cloud(p_in->points);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
