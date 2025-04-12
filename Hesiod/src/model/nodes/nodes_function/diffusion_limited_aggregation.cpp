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

void setup_diffusion_limited_aggregation_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "scale", 0.01f, 0.005f, 0.1f);
  ADD_ATTR(FloatAttribute, "seeding_radius", 0.4f, 0.1f, 0.5f);
  ADD_ATTR(FloatAttribute, "seeding_outer_radius_ratio", 0.2f, 0.01f, 0.5f);
  ADD_ATTR(FloatAttribute, "slope", 8.f, 0.1f, FLT_MAX);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.2f, 0.f, 1.f);
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "scale",
                                "seeding_radius",
                                "seeding_outer_radius_ratio",
                                "slope",
                                "noise_ratio",
                                "_SEPARATOR_",
                                "inverse",
                                "remap"});
}

void compute_diffusion_limited_aggregation_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  hmap::Array array = hmap::diffusion_limited_aggregation(
      p_node->get_config_ref()->shape,
      GET("scale", FloatAttribute),
      GET("seed", SeedAttribute),
      GET("seeding_radius", FloatAttribute),
      GET("seeding_outer_radius_ratio", FloatAttribute),
      GET("slope", FloatAttribute),
      GET("noise_ratio", FloatAttribute));

  p_out->from_array_interp_nearest(array);

  // post-process
  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
