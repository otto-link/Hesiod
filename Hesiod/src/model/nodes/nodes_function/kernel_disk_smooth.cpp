/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/kernels.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_kernel_disk_smooth_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Array>(gnode::PortType::OUT,
                             "kernel",
                             node.get_config_ref()->shape);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "radius", 0.1f, 0.001f, 0.2f);
  ADD_ATTR(node, BoolAttribute, "normalize", false);
  ADD_ATTR(node, FloatAttribute, "r_cutoff", 0.9f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"radius", "normalize", "_SEPARATOR_", "r_cutoff"});
}

void compute_kernel_disk_smooth_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Array *p_out = node.get_value_ref<hmap::Array>("kernel");

  int ir = std::max(
      1,
      (int)(GET(node, "radius", FloatAttribute) * node.get_config_ref()->shape.x));

  // kernel definition
  hmap::Vec2<int> kernel_shape = {2 * ir + 1, 2 * ir + 1};

  *p_out = hmap::disk_smooth(kernel_shape, GET(node, "r_cutoff", FloatAttribute));

  if (GET(node, "normalize", BoolAttribute))
    *p_out /= p_out->sum();

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
