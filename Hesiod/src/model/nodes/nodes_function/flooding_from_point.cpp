/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_flooding_from_point_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "depth_min", 0.01f, 0.f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"depth_min"});
}

void compute_flooding_from_point_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("elevation");
  hmap::Cloud     *p_cloud = p_node->get_value_ref<hmap::Cloud>("cloud");

  if (p_in && p_cloud)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_out, p_in},
        [p_node, p_cloud](std::vector<hmap::Array *> p_arrays,
                          hmap::Vec2<int>            shape,
                          hmap::Vec4<float>          bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_in = p_arrays[1];

          // convert point positions to cell indices
          std::vector<int> i, j;

          for (const auto &p : p_cloud->points)
          {
            int ip = static_cast<int>((p.x - bbox.a) / (bbox.b - bbox.a) * (shape.x - 1));
            int jp = static_cast<int>((p.y - bbox.c) / (bbox.d - bbox.c) * (shape.y - 1));

            if (ip >= 0 && ip <= shape.x - 1 && jp >= 0 && jp <= shape.y - 1)
            {
              i.push_back(ip);
              j.push_back(jp);
            }

            LOG->debug("{} {} {} {} {} {}", p.x, p.y, ip, jp, shape.x, shape.y);
          }

          LOG->debug("{} {} {}", i.size(), bbox.a, bbox.b);

          *pa_out = hmap::flooding_from_point(*pa_in,
                                              i,
                                              j,
                                              GET("depth_min", FloatAttribute));
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
