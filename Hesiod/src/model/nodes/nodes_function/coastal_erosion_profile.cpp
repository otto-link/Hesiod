/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_coastal_erosion_profile_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation_in");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "water_depth_in");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "elevation", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "shore_mask", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("shore_ground_extent",
                                "Ground Shore Width",
                                0.05f,
                                0.f,
                                0.2f);
  node.add_attr<FloatAttribute>("shore_water_extent_ratio",
                                "Water Shore Width Ratio",
                                1.f,
                                0.f,
                                10.f);
  node.add_attr<FloatAttribute>("scarp_extent_ratio", "Scarp Strength", 0.5f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("slope_shore", "Shore Slope", 0.5f, 0.f, 8.f);
  node.add_attr<BoolAttribute>("apply_post_filter", "Enable Shore Smoothing", true);
  node.add_attr<BoolAttribute>("distributed", "Enable Parallel Processing", true);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Shore Geometry",
                             "shore_ground_extent",
                             "shore_water_extent_ratio",
                             "_GROUPBOX_END_"
                             //
                             "_GROUPBOX_BEGIN_Profile Shape",
                             "scarp_extent_ratio",
                             "slope_shore",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Smoothing",
                             "apply_post_filter",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Performances",
                             "distributed",
                             "_GROUPBOX_END_"});
}

void compute_coastal_erosion_profile_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_z = node.get_value_ref<hmap::Heightmap>("elevation_in");
  hmap::Heightmap *p_depth = node.get_value_ref<hmap::Heightmap>("water_depth_in");

  if (p_z && p_depth)
  {
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_z_out = node.get_value_ref<hmap::Heightmap>("elevation");
    hmap::Heightmap *p_depth_out = node.get_value_ref<hmap::Heightmap>("water_depth");
    hmap::Heightmap *p_shore_mask = node.get_value_ref<hmap::Heightmap>("shore_mask");

    hmap::TransformMode transform_mode = node.get_attr<BoolAttribute>("distributed")
                                             ? node.get_config_ref()
                                                   ->hmap_transform_mode_cpu
                                             : hmap::TransformMode::SINGLE_ARRAY;

    int ir_ground = std::max(
        1,
        (int)(node.get_attr<FloatAttribute>("shore_ground_extent") * p_z_out->shape.x));

    int ir_water = int(node.get_attr<FloatAttribute>("shore_water_extent_ratio") *
                       ir_ground);

    float slope_n = node.get_attr<FloatAttribute>("slope_shore") /
                    float(p_z_out->shape.x);

    hmap::transform(
        {p_z, p_depth, p_mask, p_z_out, p_depth_out, p_shore_mask},
        [&node, ir_ground, ir_water, slope_n](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_z = p_arrays[0];
          hmap::Array *pa_depth = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];
          hmap::Array *pa_z_out = p_arrays[3];
          hmap::Array *pa_depth_out = p_arrays[4];
          hmap::Array *pa_shore_mask = p_arrays[5];

          *pa_z_out = *pa_z;
          *pa_depth_out = *pa_depth;

          hmap::coastal_erosion_profile(
              *pa_z_out,
              pa_mask,
              *pa_depth_out,
              ir_ground,
              ir_water,
              slope_n * float(pa_z->shape.x),
              slope_n * float(pa_z->shape.x),
              node.get_attr<FloatAttribute>("scarp_extent_ratio"),
              node.get_attr<BoolAttribute>("apply_post_filter"),
              pa_shore_mask);
        },

        transform_mode);

    p_z_out->smooth_overlap_buffers();
    p_depth_out->smooth_overlap_buffers();
    p_shore_mask->smooth_overlap_buffers();
  }
}

} // namespace hesiod
