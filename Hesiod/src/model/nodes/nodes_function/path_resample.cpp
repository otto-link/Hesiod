/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_resample_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "input");
  node.add_port<hmap::Path>(gnode::PortType::OUT, "output");

  // attribute(s)
  node.add_attr<FloatAttribute>("delta", "delta", 0.01f, 0.001f, 0.2f);
}

void compute_path_resample_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_in = node.get_value_ref<hmap::Path>("input");

  if (p_in)
  {
    hmap::Path *p_out = node.get_value_ref<hmap::Path>("output");
    // copy the input heightmap
    *p_out = *p_in;

    if (p_in->get_npoints() > 1)
      p_out->resample(node.get_attr<FloatAttribute>("delta"));
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
