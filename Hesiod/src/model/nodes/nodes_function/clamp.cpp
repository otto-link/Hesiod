/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_clamp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<RangeAttribute>("clamp", "clamp");
  node.add_attr<BoolAttribute>("smooth_min", "smooth_min", false);
  node.add_attr<FloatAttribute>("k_min", "k_min", 0.05f, 0.01f, 1.f);
  node.add_attr<BoolAttribute>("smooth_max", "smooth_max", false);
  node.add_attr<FloatAttribute>("k_max", "k_max", 0.05f, 0.01f, 1.f);
  node.add_attr<BoolAttribute>("remap", "remap", false);

  // link histogram for RangeAttribute
  setup_histogram_for_range_attribute(node, "clamp", "input");

  // attribute(s) order
  node.set_attr_ordered_key(
      {"clamp", "smooth_min", "k_min", "smooth_max", "k_max", "remap"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = false});
}

void compute_clamp_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    // retrieve parameters
    hmap::Vec2<float> crange = node.get_attr<RangeAttribute>("clamp");
    bool              smooth_min = node.get_attr<BoolAttribute>("smooth_min");
    bool              smooth_max = node.get_attr<BoolAttribute>("smooth_max");
    float             k_min = node.get_attr<FloatAttribute>("k_min");
    float             k_max = node.get_attr<FloatAttribute>("k_max");

    // compute
    if (!smooth_min && !smooth_max)
    {
      hmap::transform(
          {p_out},
          [&crange](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::clamp(*pa_out, crange.x, crange.y);
          },
          node.get_config_ref()->hmap_transform_mode_cpu);
    }
    else
    {
      if (smooth_min)
        hmap::transform(
            {p_out},
            [&crange, &k_min](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_min_smooth(*pa_out, crange.x, k_min);
            },
            node.get_config_ref()->hmap_transform_mode_cpu);
      else
        hmap::transform(
            {p_out},
            [&crange](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_min(*pa_out, crange.x);
            },
            node.get_config_ref()->hmap_transform_mode_cpu);

      if (smooth_max)
        hmap::transform(
            {p_out},
            [&crange, &k_max](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_max_smooth(*pa_out, crange.y, k_max);
            },
            node.get_config_ref()->hmap_transform_mode_cpu);
      else
        hmap::transform(
            {p_out},
            [&crange](std::vector<hmap::Array *> p_arrays)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_max(*pa_out, crange.y);
            },
            node.get_config_ref()->hmap_transform_mode_cpu);
    }

    if (node.get_attr<BoolAttribute>("remap"))
      p_out->remap();

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
