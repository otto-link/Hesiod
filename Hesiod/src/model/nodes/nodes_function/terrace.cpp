/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_terrace_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(IntAttribute, "nlevels", 4, 1, 32);
  ADD_ATTR(FloatAttribute, "gain", 0.8f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "noise_ratio", 0.1f, 0.f, 0.5f);
  ADD_ATTR(SeedAttribute, "seed");

  // attribute(s) order
  p_node->set_attr_ordered_key({"nlevels", "gain", "noise_ratio", "seed"});

  setup_mask_attributes(p_node);
}

void compute_terrace_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = p_node->get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(p_node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(
        {p_out, p_noise, p_mask},
        [p_node, hmin, hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_noise = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          hmap::terrace(*pa_out,
                        GET("seed", SeedAttribute),
                        GET("nlevels", IntAttribute),
                        pa_mask,
                        GET("gain", FloatAttribute),
                        GET("noise_ratio", FloatAttribute),
                        pa_noise,
                        hmin,
                        hmax);
        },
        p_node->get_config_ref()->hmap_transform_mode_gpu);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
