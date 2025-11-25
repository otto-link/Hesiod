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

void setup_reverse_midpoint_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("noise_scale", "noise_scale", 1.f, 0.01f, 2.f);
  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
  node.add_attr<BoolAttribute>("remap", "remap", true);

  // attribute(s) order
  node.set_attr_ordered_key({"noise_scale", "seed", "_SEPARATOR_", "inverse", "remap"});
}

void compute_reverse_midpoint_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_path = node.get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("heightmap");

    if (p_path->get_npoints() > 1)
    {
      hmap::Array       path_array = hmap::Array(p_out->shape);
      hmap::Vec4<float> bbox = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
      p_path->to_array(path_array, bbox);

      hmap::Array z = hmap::reverse_midpoint(path_array,
                                             node.get_attr<SeedAttribute>("seed"),
                                             node.get_attr<FloatAttribute>("noise_scale"),
                                             0.f); // threshold

      p_out->from_array_interp_nearest(z);

      // post-process
      post_process_heightmap(node,
                             *p_out,
                             node.get_attr<BoolAttribute>("inverse"),
                             false, // smooth
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             node.get_attr<BoolAttribute>("remap"),
                             {0.f, 1.f});
    }
    else
      // fill with zeroes
      hmap::transform(*p_out, [](hmap::Array &array) { array = 0.f; });
  }
}

} // namespace hesiod
