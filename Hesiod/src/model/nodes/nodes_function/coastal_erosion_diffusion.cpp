/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_coastal_erosion_diffusion_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation_in");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "water_depth_in");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "elevation", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_depth", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "water_mask", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "additional_depth", 0.05f, 0.f, 0.2f);
  ADD_ATTR(IntAttribute, "iterations", 10, 0, INT_MAX);
  ADD_ATTR(BoolAttribute, "distributed", true);

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"additional_depth", "iterations", "_SEPARATOR_", "distributed"});

  add_wip_warning_label(p_node);
}

void compute_coastal_erosion_diffusion_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap *p_z = p_node->get_value_ref<hmap::Heightmap>("elevation_in");
  hmap::Heightmap *p_depth = p_node->get_value_ref<hmap::Heightmap>("water_depth_in");

  if (p_z && p_depth)
  {
    hmap::Heightmap *p_z_out = p_node->get_value_ref<hmap::Heightmap>("elevation");
    hmap::Heightmap *p_depth_out = p_node->get_value_ref<hmap::Heightmap>("water_depth");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("water_mask");

    hmap::TransformMode transform_mode = GET("distributed", BoolAttribute)
                                             ? p_node->get_config_ref()
                                                   ->hmap_transform_mode_cpu
                                             : hmap::TransformMode::SINGLE_ARRAY;

    hmap::transform(
        {p_depth, p_z, p_depth_out, p_z_out, p_mask},
        [p_node](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_depth = p_arrays[0];
          hmap::Array *pa_z = p_arrays[1];
          hmap::Array *pa_depth_out = p_arrays[2];
          hmap::Array *pa_z_out = p_arrays[3];
          hmap::Array *pa_mask = p_arrays[4];

          *pa_z_out = *pa_z;
          *pa_depth_out = *pa_depth;

          hmap::coastal_erosion_diffusion(*pa_z_out,
                                          *pa_depth_out,
                                          GET("additional_depth", FloatAttribute),
                                          GET("iterations", IntAttribute),
                                          pa_mask);
        },
        transform_mode);

    p_z_out->smooth_overlap_buffers();
    p_depth_out->smooth_overlap_buffers();
    p_mask->smooth_overlap_buffers();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
