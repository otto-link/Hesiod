/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_to_array_interp_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG);
}

void compute_cloud_to_array_interp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Cloud *p_cloud = p_node->get_value_ref<hmap::Cloud>("cloud");

  if (p_cloud)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("heightmap");

    if (p_cloud->get_npoints() > 0)
    {
      hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
      hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");

      hmap::fill(*p_out,
                 p_dx,
                 p_dy,
                 [p_node, p_cloud](hmap::Vec2<int>   shape,
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

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
