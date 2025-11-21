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

void setup_ridgelines_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Path>(gnode::PortType::IN, "path");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "heightmap", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("talus_global", "talus_global", 4.f, -FLT_MAX, FLT_MAX);

  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 1.f, 0.01f, 2.f);

  node.add_attr<FloatAttribute>("width", "width", 0.1f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("vmin", "vmin", 0.f, -1.f, 1.f);
}

void compute_ridgelines_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Path *p_path = node.get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("heightmap");

    if (p_path->get_npoints() > 1)
    {
      hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
      hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");

      std::vector<float> xs, ys, zs = {};

      for (size_t k = 0; k < p_path->get_npoints() - 1; k++)
      {
        xs.push_back(p_path->points[k].x);
        ys.push_back(p_path->points[k].y);
        zs.push_back(p_path->points[k].v);

        xs.push_back(p_path->points[k + 1].x);
        ys.push_back(p_path->points[k + 1].y);
        zs.push_back(p_path->points[k + 1].v);
      }

      hmap::fill(*p_out,
                 p_dx,
                 p_dy,
                 [&node, p_path, xs, ys, zs](hmap::Vec2<int>   shape,
                                             hmap::Vec4<float> bbox,
                                             hmap::Array      *p_noise_x,
                                             hmap::Array      *p_noise_y)
                 {
                   hmap::Array       array(shape);
                   hmap::Vec4<float> bbox_points = {0.f, 1.f, 0.f, 1.f};

                   array = hmap::ridgelines(shape,
                                            xs,
                                            ys,
                                            zs,
                                            node.get_attr<FloatAttribute>("talus_global"),
                                            node.get_attr<FloatAttribute>("k_smoothing"),
                                            node.get_attr<FloatAttribute>("width"),
                                            node.get_attr<FloatAttribute>("vmin"),
                                            bbox_points,
                                            p_noise_x,
                                            p_noise_y,
                                            nullptr, // stretching
                                            bbox);
                   return array;
                 });
    }
    else
      // fill with zeroes
      hmap::transform(*p_out, [](hmap::Array &array) { array = 0.f; });
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
