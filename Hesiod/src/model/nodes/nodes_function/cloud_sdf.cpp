/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_sdf_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "sdf", CONFIG(node));

  setup_post_process_heightmap_attributes(node);
}

void compute_cloud_sdf_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud     *p_cloud = node.get_value_ref<hmap::Cloud>("cloud");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("sdf");

  if (!p_cloud || p_cloud->get_npoints() == 0)
  {
    // fill with zeros
    hmap::transform(*p_out, [](hmap::Array &x) { x = 0.f; });
  }
  else
  {
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");

    hmap::fill(
        *p_out,
        p_dx,
        p_dy,
        [&node, p_cloud](hmap::Vec2<int>   shape,
                         hmap::Vec4<float> bbox,
                         hmap::Array      *p_noise_x,
                         hmap::Array      *p_noise_y)
        {
          hmap::Vec4<float> bbox_full = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
          return p_cloud->to_array_sdf(shape, bbox_full, p_noise_x, p_noise_y, bbox);
        });

    // post-process
    post_process_heightmap(node, *p_out);
  }
}

} // namespace hesiod
