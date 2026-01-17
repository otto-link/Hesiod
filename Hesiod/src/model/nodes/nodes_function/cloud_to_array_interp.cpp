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
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dx");
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "dy");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "heightmap", CONFIG2(node));
}

void compute_cloud_to_array_interp_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_cloud = node.get_value_ref<hmap::Cloud>("cloud");

  if (p_cloud)
  {
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("heightmap");

    if (p_cloud->get_npoints() > 0)
    {
      hmap::VirtualArray *p_dx = node.get_value_ref<hmap::VirtualArray>("dx");
      hmap::VirtualArray *p_dy = node.get_value_ref<hmap::VirtualArray>("dy");

      hmap::for_each_tile(
          {p_out, p_dx, p_dy},
          [&node, p_cloud](std::vector<hmap::Array *> p_arrays,
                           const hmap::TileRegion    &region)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_dx = p_arrays[1];
            hmap::Array *pa_dy = p_arrays[2];

            hmap::Vec4<float> bbox_points = {0.f, 1.f, 0.f, 1.f};

            *pa_out = 0.f;

            p_cloud->to_array_interp(*pa_out,
                                     bbox_points,
                                     hmap::InterpolationMethod2D::DELAUNAY,
                                     pa_dx,
                                     pa_dy,
                                     region.bbox);
          },
          node.cfg().cm_cpu);
    }
    else
    {
      // fill with zeroes
      hmap::for_each_tile(
          {p_out},
          [](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
          {
            hmap::Array *pa_out = p_arrays[0];
            *pa_out = 0.f;
          },
          node.cfg().cm_cpu);
    }
  }
}

} // namespace hesiod
