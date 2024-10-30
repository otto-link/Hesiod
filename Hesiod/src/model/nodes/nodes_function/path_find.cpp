/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_path_find_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "waypoints");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "heightmap");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask nogo");
  p_node->add_port<hmap::Path>(gnode::PortType::OUT, "path");

  // attribute(s)
  p_node->add_attr<FloatAttribute>("elevation_ratio", 0.1f, 0.f, 0.9f, "elevation_ratio");
  p_node->add_attr<FloatAttribute>("distance_exponent",
                                   1.f,
                                   0.5f,
                                   2.f,
                                   "distance_exponent");
  p_node->add_attr<IntAttribute>("downsampling", 4, 1, 10, "downsampling");

  // attribute(s) order
  p_node->set_attr_ordered_key({"elevation_ratio", "distance_exponent", "downsampling"});
}

void compute_path_find_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Path      *p_waypoints = p_node->get_value_ref<hmap::Path>("waypoints");
  hmap::HeightMap *p_hmap = p_node->get_value_ref<hmap::HeightMap>("heightmap");

  if (p_waypoints && p_hmap)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask nogo");
    hmap::Path      *p_out = p_node->get_value_ref<hmap::Path>("path");

    // copy the input heightmap
    *p_out = *p_waypoints;

    if (p_out->get_npoints() > 1)
    {
      // working shape
      float           ds = (float)GET("downsampling", IntAttribute);
      hmap::Vec2<int> shape_wrk = hmap::Vec2<int>((int)(p_hmap->shape.x / ds),
                                                  (int)(p_hmap->shape.y / ds));

      shape_wrk.x = std::max(2, shape_wrk.x);
      shape_wrk.y = std::max(2, shape_wrk.y);

      LOG->trace("working shape: ({}, {})", shape_wrk.x, shape_wrk.y);

      // work on a single array (as a temporary solution?)
      hmap::Array z = p_hmap->to_array();
      hmap::Array zw = z.resample_to_shape_nearest(shape_wrk);

      // handle masking
      hmap::Array *p_mask_array = nullptr;
      hmap::Array  mask_array;

      if (p_mask)
      {
        mask_array = p_mask->to_array();
        mask_array = mask_array.resample_to_shape_nearest(shape_wrk);
        p_mask_array = &mask_array;
      }

      // perform Dijkstra's path finding
      hmap::Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);
      int               edge_divisions = 0;

      p_out->dijkstra(zw,
                      bbox,
                      edge_divisions,
                      GET("elevation_ratio", FloatAttribute),
                      GET("distance_exponent", FloatAttribute),
                      p_mask_array);

      // set values based on the "fine" grid array
      p_out->set_values_from_array(z, bbox);
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
