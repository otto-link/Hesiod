/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/hydrology.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_flow_stream_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Cloud>(gnode::PortType::IN, "sources");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "river_mask", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("elevation_ratio",
                                   0.5f,
                                   0.f,
                                   0.95f,
                                   "elevation_ratio");
  p_node->add_attr<FloatAttribute>("distance_exponent",
                                   2.f,
                                   0.1f,
                                   4.f,
                                   "distance_exponent");
  p_node->add_attr<FloatAttribute>("upward_penalization",
                                   100.f,
                                   1.f,
                                   1000.f,
                                   "upward_penalization");
  p_node->add_attr<FloatAttribute>("riverbank_slope", 1.f, 0.f, 16.f, "riverbank_slope");
  p_node->add_attr<FloatAttribute>("river_radius", 0.001f, 0.001f, 0.1f, "river_radius");
  p_node->add_attr<FloatAttribute>("depth", 0.01f, 0.f, 0.2f, "depth");
  p_node->add_attr<FloatAttribute>("merging_radius",
                                   0.001f,
                                   0.f,
                                   0.1f,
                                   "merging_radius");
  p_node->add_attr<FloatAttribute>("riverbed_slope", 0.01f, 0.f, 0.1f, "riverbed_slope");
  p_node->add_attr<FloatAttribute>("noise_ratio", 0.9f, 0.f, 1.f, "noise_ratio");
  p_node->add_attr<SeedAttribute>("seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"elevation_ratio",
                                "distance_exponent",
                                "upward_penalization",
                                "_SEPARATOR_",
                                "riverbank_slope",
				"river_radius",
				"depth",
                                "merging_radius",
                                "riverbed_slope",
                                "_SEPARATOR_",
                                "noise_ratio",
                                "seed"});
}

void compute_flow_stream_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");
  hmap::Cloud     *p_cloud = p_node->get_value_ref<hmap::Cloud>("sources");

  if (p_in && p_cloud)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("river_mask");

    *p_out = *p_in;

    hmap::transform(
        {p_out, p_mask},
        [p_node, p_cloud](std::vector<hmap::Array *> p_arrays,
                          hmap::Vec2<int>            shape,
                          hmap::Vec4<float>)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_mask = p_arrays[1];

          // find a flow stream for each source
          std::vector<hmap::Path> path_list;

          for (auto p : p_cloud->points)
          {
            int             i = (int)(p.x * (shape.x - 1.f));
            int             j = (int)(p.y * (shape.y - 1.f));
            hmap::Vec2<int> ij_start(i, j);

            hmap::Path path = hmap::flow_stream(
                *pa_out,
                ij_start,
                GET("elevation_ratio", FloatAttribute),
                GET("distance_exponent", FloatAttribute),
                GET("upward_penalization", FloatAttribute));

            path_list.push_back(path);
          }

          // dig the rivers
          int merging_ir = int(GET("merging_radius", FloatAttribute) * (shape.x - 1.f));
	  merging_ir = std::max(1, merging_ir);

	  int river_ir = int(GET("river_radius", FloatAttribute) * (shape.x - 1.f));
	  
          hmap::dig_river(*pa_out,
                          path_list,
                          GET("riverbank_slope", FloatAttribute) / (shape.x - 1.f),
			  river_ir,
                          merging_ir,
			  GET("depth", FloatAttribute),
                          GET("riverbed_slope", FloatAttribute),
                          GET("noise_ratio", FloatAttribute),
                          GET("seed", SeedAttribute),
                          pa_mask);
        },
        hmap::TransformMode::SINGLE_ARRAY);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
