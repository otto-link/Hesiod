/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_diffusion_limited_aggregation_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "scale", 0.01f, 0.005f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "seeding_radius", 0.4f, 0.1f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "seeding_outer_radius_ratio", 0.2f, 0.01f, 0.5f);
  ADD_ATTR(node, FloatAttribute, "slope", 8.f, 0.1f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "noise_ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(node, BoolAttribute, "inverse", false);
  ADD_ATTR(node, RangeAttribute, "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
                             "scale",
                             "seeding_radius",
                             "seeding_outer_radius_ratio",
                             "slope",
                             "noise_ratio",
                             "_SEPARATOR_",
                             "inverse",
                             "remap"});
}

void compute_diffusion_limited_aggregation_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::Array array = hmap::diffusion_limited_aggregation(
      node.get_config_ref()->shape,
      GET(node, "scale", FloatAttribute),
      GET(node, "seed", SeedAttribute),
      GET(node, "seeding_radius", FloatAttribute),
      GET(node, "seeding_outer_radius_ratio", FloatAttribute),
      GET(node, "slope", FloatAttribute),
      GET(node, "noise_ratio", FloatAttribute));

  p_out->from_array_interp_nearest(array);

  // post-process
  post_process_heightmap(node,
                         *p_out,
                         GET(node, "inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_MEMBER(node, "remap", RangeAttribute, is_active),
                         GET(node, "remap", RangeAttribute));

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
