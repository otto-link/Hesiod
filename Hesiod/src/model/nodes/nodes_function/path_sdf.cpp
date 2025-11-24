/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/path.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_sdf_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "sdf", CONFIG(node));

  // attribute(s)
  node.add_attr<BoolAttribute>("remap", "remap", false);
  node.add_attr<BoolAttribute>("inverse", "inverse", false);
}

void compute_path_sdf_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_path = node.get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("sdf");

    if (p_path->get_npoints() > 1)
    {
      hmap::fill(
          *p_out,
          p_dx,
          p_dy,
          [p_path](hmap::Vec2<int>   shape,
                   hmap::Vec4<float> bbox,
                   hmap::Array      *p_noise_x,
                   hmap::Array      *p_noise_y)
          {
            hmap::Vec4<float> bbox_full = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
            return p_path->to_array_sdf(shape, bbox_full, p_noise_x, p_noise_y, bbox);
          });

      // post-process
      post_process_heightmap(node,
                             *p_out,
                             node.get_attr<BoolAttribute>("inverse"),
                             false, // smoothing,
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             node.get_attr<BoolAttribute>("remap"),
                             {0.f, 1.f});
    }
    else
      // fill with zeros
      hmap::transform(*p_out, [](hmap::Array x) { x = 0.f; });
  }
}

} // namespace hesiod
