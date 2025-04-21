/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_reverse_midpoint_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "path");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "noise_scale", 1.f, 0.01f, 2.f);
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(BoolAttribute, "remap", true);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"noise_scale", "seed", "_SEPARATOR_", "inverse", "remap"});
}

void compute_reverse_midpoint_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_path = p_node->get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("heightmap");

    if (p_path->get_npoints() > 1)
    {
      hmap::Array       path_array = hmap::Array(p_out->shape);
      hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
      p_path->to_array(path_array, bbox);

      hmap::Array z = hmap::reverse_midpoint(path_array,
                                             GET("seed", SeedAttribute),
                                             GET("noise_scale", FloatAttribute),
                                             0.f); // threshold

      p_out->from_array_interp_nearest(z);

      // post-process
      post_process_heightmap(p_node,
                             *p_out,
                             GET("inverse", BoolAttribute),
                             false, // smooth
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             GET("remap", BoolAttribute),
                             {0.f, 1.f});
    }
    else
      // fill with zeroes
      hmap::transform(*p_out, [](hmap::Array &array) { array = 0.f; });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
