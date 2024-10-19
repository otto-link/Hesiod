/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/authoring.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_ridgelines_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "path");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "heightmap", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("talus_global", 4.f, -16.f, 16.f, "talus_global");
  p_node->add_attr<FloatAttribute>("k_smoothing", 1.f, 0.01f, 2.f, "k_smoothing");
  p_node->add_attr<FloatAttribute>("width", 0.1f, 0.f, 1.f, "width");
  p_node->add_attr<FloatAttribute>("vmin", 0.f, -1.f, 1.f, "vmin");
}

void compute_ridgelines_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path *p_path = p_node->get_value_ref<hmap::Path>("path");

  if (p_path)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("heightmap");

    if (p_path->get_npoints() > 1)
    {
      hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
      hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");

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
                 [p_node, p_path, xs, ys, zs](hmap::Vec2<int>   shape,
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
                                            GET("talus_global", FloatAttribute),
                                            GET("k_smoothing", FloatAttribute),
                                            GET("width", FloatAttribute),
                                            GET("vmin", FloatAttribute),
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

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
