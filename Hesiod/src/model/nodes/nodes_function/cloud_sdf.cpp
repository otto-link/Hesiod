/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_cloud_sdf_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "sdf", CONFIG);

  // attribute(s)
  p_node->add_attr<BoolAttribute>("remap", false, "remap");
  p_node->add_attr<BoolAttribute>("inverse", false, "inverse");

  // attribute(s) order
  p_node->set_attr_ordered_key({"remap", "inverse"});
}

void compute_cloud_sdf_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Cloud     *p_cloud = p_node->get_value_ref<hmap::Cloud>("cloud");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("sdf");

  if (p_cloud)
  {
    if (p_cloud->get_npoints() > 1)
    {
      hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
      hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");

      hmap::fill(
          *p_out,
          p_dx,
          p_dy,
          [p_node, p_cloud](hmap::Vec2<int>   shape,
                            hmap::Vec4<float> bbox,
                            hmap::Array      *p_noise_x,
                            hmap::Array      *p_noise_y)
          {
            hmap::Vec4<float> bbox_full = hmap::Vec4<float>(0.f, 1.f, 0.f, 1.f);
            return p_cloud->to_array_sdf(shape, bbox_full, p_noise_x, p_noise_y, bbox);
          });

      // post-process
      post_process_heightmap(p_node,
                             *p_out,
                             GET("inverse", BoolAttribute),
                             false, // smoothing,
                             0,
                             false, // saturate
                             {0.f, 0.f},
                             0.f,
                             GET("remap", BoolAttribute),
                             {0.f, 1.f});
    }
  }
  else
    // fill with zeros
    hmap::transform(*p_out, [](hmap::Array x) { x = 0.f; });

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
