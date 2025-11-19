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

void setup_flooding_from_point_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "depth_min", 0.01f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"depth_min"});
}

void compute_flooding_from_point_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("elevation");
  hmap::Cloud     *p_cloud = node.get_value_ref<hmap::Cloud>("cloud");

  if (p_in && p_cloud)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("water_depth");

    hmap::transform(
        {p_out, p_in},
        [&node, p_cloud](std::vector<hmap::Array *> p_arrays,
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
          }

          *pa_out = hmap::flooding_from_point(*pa_in,
                                              i,
                                              j,
                                              GET(node, "depth_min", FloatAttribute));
        },
        hmap::TransformMode::SINGLE_ARRAY); // forced, not tileable
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
