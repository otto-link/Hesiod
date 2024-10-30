/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/filters.hpp"
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
  p_node->add_attr<FloatAttribute>("elevation_ratio", 0.5f, 0.f, 0.9f, "elevation_ratio");
  p_node->add_attr<FloatAttribute>("distance_exponent",
                                   2.f,
                                   0.5f,
                                   2.f,
                                   "distance_exponent");

  // attribute(s) order
  p_node->set_attr_ordered_key({"elevation_ratio", "distance_exponent"});
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
      // work on a single array (as a temporary solution?)
      hmap::Array z = p_hmap->to_array();

      // handle masking
      hmap::Array *p_mask_array = nullptr;
      hmap::Array  mask_array;

      if (p_mask)
      {
        mask_array = p_mask->to_array();
        p_mask_array = &mask_array;
      }

      // perform Dijkstra's path finding
      hmap::Vec4<float> bbox(0.f, 1.f, 0.f, 1.f);
      int               edge_divisions = 0;

      p_out->dijkstra(z,
                      bbox,
                      edge_divisions,
                      GET("elevation_ratio", FloatAttribute),
                      GET("distance_exponent", FloatAttribute),
                      p_mask_array);
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
