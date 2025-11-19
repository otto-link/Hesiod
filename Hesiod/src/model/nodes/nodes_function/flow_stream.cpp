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

void setup_flow_stream_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "sources");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "river_mask", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "elevation_ratio", 0.5f, 0.f, 0.95f);
  ADD_ATTR(node, FloatAttribute, "distance_exponent", 2.f, 0.1f, 4.f);
  ADD_ATTR(node, FloatAttribute, "upward_penalization", 100.f, 1.f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "riverbank_slope", 1.f, 0.f, 16.f);
  ADD_ATTR(node, FloatAttribute, "river_radius", 0.001f, 0.001f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "depth", 0.01f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "merging_radius", 0.001f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "riverbed_slope", 0.01f, 0.f, 0.1f);
  ADD_ATTR(node, FloatAttribute, "noise_ratio", 0.9f, 0.f, 1.f);
  ADD_ATTR(node, SeedAttribute, "seed");

  // attribute(s) order
  node.set_attr_ordered_key({"elevation_ratio",
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

void compute_flow_stream_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");
  hmap::Cloud     *p_cloud = node.get_value_ref<hmap::Cloud>("sources");

  if (p_in && p_cloud)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("river_mask");

    *p_out = *p_in;

    hmap::transform(
        {p_out, p_mask},
        [&node, p_cloud](std::vector<hmap::Array *> p_arrays,
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
                GET(node, "elevation_ratio", FloatAttribute),
                GET(node, "distance_exponent", FloatAttribute),
                GET(node, "upward_penalization", FloatAttribute));

            path_list.push_back(path);
          }

          // dig the rivers
          int merging_ir = int(GET(node, "merging_radius", FloatAttribute) *
                               (shape.x - 1.f));
          merging_ir = std::max(1, merging_ir);

          int river_ir = int(GET(node, "river_radius", FloatAttribute) * (shape.x - 1.f));

          hmap::dig_river(*pa_out,
                          path_list,
                          GET(node, "riverbank_slope", FloatAttribute) / (shape.x - 1.f),
                          river_ir,
                          merging_ir,
                          GET(node, "depth", FloatAttribute),
                          GET(node, "riverbed_slope", FloatAttribute),
                          GET(node, "noise_ratio", FloatAttribute),
                          GET(node, "seed", SeedAttribute),
                          pa_mask);
        },
        hmap::TransformMode::SINGLE_ARRAY);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
