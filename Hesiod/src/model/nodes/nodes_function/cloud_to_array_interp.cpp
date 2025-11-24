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

void setup_cloud_to_array_interp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG(node));
}

void compute_cloud_to_array_interp_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_cloud = node.get_value_ref<hmap::Cloud>("cloud");

  if (p_cloud)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("heightmap");

    if (p_cloud->get_npoints() > 0)
    {
      hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
      hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");

      hmap::fill(*p_out,
                 p_dx,
                 p_dy,
                 [&node, p_cloud](hmap::Vec2<int>   shape,
                                  hmap::Vec4<float> bbox,
                                  hmap::Array      *p_noise_x,
                                  hmap::Array      *p_noise_y)
                 {
                   hmap::Array       array(shape);
                   hmap::Vec4<float> bbox_points = {0.f, 1.f, 0.f, 1.f};

                   p_cloud->to_array_interp(array,
                                            bbox_points,
                                            hmap::InterpolationMethod2D::DELAUNAY,
                                            p_noise_x,
                                            p_noise_y,
                                            bbox);
                   return array;
                 });
    }
    else
      // fill with zeroes
      hmap::transform(*p_out, [](hmap::Array &array) { array = 0.f; });
  }
}

} // namespace hesiod
