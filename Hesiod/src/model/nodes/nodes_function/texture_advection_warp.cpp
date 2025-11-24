/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "highmap/transform.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_texture_advection_warp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "elevation");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::IN, "texture");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::HeightmapRGBA>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("advection_length", "advection_length", 0.05f, 0.f, 0.2f);
  node.add_attr<FloatAttribute>("value_persistence",
                                "value_persistence",
                                0.95f,
                                0.8f,
                                1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"advection_length", "value_persistence"});

  setup_pre_process_mask_attributes(node);
}

void compute_texture_advection_warp_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap     *p_z = node.get_value_ref<hmap::Heightmap>("elevation");
  hmap::HeightmapRGBA *p_tex = node.get_value_ref<hmap::HeightmapRGBA>("texture");

  if (p_z && p_tex)
  {
    hmap::Heightmap     *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::HeightmapRGBA *p_out = node.get_value_ref<hmap::HeightmapRGBA>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_z);

    // apply advection separetely to each RGBA channels
    auto lambda = [&node](hmap::Heightmap *p_z,
                          hmap::Heightmap *p_field,
                          hmap::Heightmap *p_mask,
                          hmap::Heightmap *p_field_out)
    {
      hmap::transform(
          {p_field_out, p_z, p_field, p_mask},
          [&node](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_field_out = p_arrays[0];
            hmap::Array *pa_z = p_arrays[1];
            hmap::Array *pa_field = p_arrays[2];
            hmap::Array *pa_mask = p_arrays[3];

            *pa_field_out = hmap::gpu::advection_warp(
                *pa_z,
                *pa_field,
                node.get_attr<FloatAttribute>("advection_length"),
                node.get_attr<FloatAttribute>("value_persistence"),
                pa_mask);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    };

    for (int nch = 0; nch < 4; nch++)
    {
      lambda(p_z, &(p_tex->rgba[nch]), p_mask, &(p_out->rgba[nch]));
      p_out->rgba[nch].smooth_overlap_buffers();
    }
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
