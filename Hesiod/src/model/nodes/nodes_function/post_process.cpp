/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_post_process_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "in");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)

  // attribute(s) order
  node.set_attr_ordered_key({});

  setup_post_process_heightmap_attributes(node);
}

void compute_post_process_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base post_process function
  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("in");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");

    // copy and post-process
    *p_out = *p_in;
    post_process_heightmap(node, *p_out);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
