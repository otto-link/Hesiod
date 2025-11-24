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

void setup_terrace_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "noise");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<IntAttribute>("nlevels", "nlevels", 4, 1, 32);
  node.add_attr<FloatAttribute>("gain", "gain", 0.8f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("noise_ratio", "noise_ratio", 0.1f, 0.f, 0.5f);
  node.add_attr<SeedAttribute>("seed", "seed");

  // attribute(s) order
  node.set_attr_ordered_key({"nlevels", "gain", "noise_ratio", "seed"});

  setup_pre_process_mask_attributes(node);
}

void compute_terrace_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_noise = node.get_value_ref<hmap::Heightmap>("noise");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // prepare mask
    std::shared_ptr<hmap::Heightmap> sp_mask = pre_process_mask(node, p_mask, *p_in);

    // copy the input heightmap
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    hmap::transform(
        {p_out, p_noise, p_mask},
        [&node, hmin, hmax](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_noise = p_arrays[1];
          hmap::Array *pa_mask = p_arrays[2];

          hmap::terrace(*pa_out,
                        node.get_attr<SeedAttribute>("seed"),
                        node.get_attr<IntAttribute>("nlevels"),
                        pa_mask,
                        node.get_attr<FloatAttribute>("gain"),
                        node.get_attr<FloatAttribute>("noise_ratio"),
                        pa_noise,
                        hmin,
                        hmax);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);
  }
}

} // namespace hesiod
