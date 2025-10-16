/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_texture_advection_warp_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(FloatAttribute, "advection_length", 0.05f, 0.f, 0.2f);
  ADD_ATTR(FloatAttribute, "value_persistence", 0.95f, 0.8f, 1.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"advection_length", "value_persistence"});

  setup_pre_process_mask_attributes(p_node);

  add_wip_warning_label(p_node);
}

void compute_texture_advection_warp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Heightmap     *p_z = p_node->get_value_ref<hmap::Heightmap>("elevation");
  hmap::HeightmapRGBA *p_tex = p_node->get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_z && p_tex)
  {
    hmap::Heightmap     *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::HeightmapRGBA *p_out = p_node->get_value_ref<hmap::HeightmapRGBA>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(p_node, p_mask, *p_z);

    // apply advection separetely to each RGBA channels
    auto lambda = [p_node](hmap::Heightmap *p_z,
                           hmap::Heightmap *p_field,
                           hmap::Heightmap *p_mask,
                           hmap::Heightmap *p_field_out)
    {
      hmap::transform(
          {p_field_out, p_z, p_field, p_mask},
          [p_node](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_field_out = p_arrays[0];
            hmap::Array *pa_z = p_arrays[1];
            hmap::Array *pa_field = p_arrays[2];
            hmap::Array *pa_mask = p_arrays[3];

            *pa_field_out = hmap::gpu::advection_warp(
                *pa_z,
                *pa_field,
                GET("advection_length", FloatAttribute),
                GET("value_persistence", FloatAttribute),
                pa_mask);
          },
          p_node->get_config_ref()->hmap_transform_mode_gpu);
    };

    for (int nch = 0; nch < 4; nch++)
    {
      lambda(p_z, &(p_tex->rgba[nch]), p_mask, &(p_out->rgba[nch]));
      p_out->rgba[nch].smooth_overlap_buffers();
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
