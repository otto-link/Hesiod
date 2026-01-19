/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
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
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "input");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "output", CONFIG(node));

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

  hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>("input");

  if (p_in)
  {
    hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("output");

    // retrieve parameters
    glm::vec2 crange = node.get_attr<RangeAttribute>("clamp");
    bool      smooth_min = node.get_attr<BoolAttribute>("smooth_min");
    bool      smooth_max = node.get_attr<BoolAttribute>("smooth_max");
    float     k_min = node.get_attr<FloatAttribute>("k_min");
    float     k_max = node.get_attr<FloatAttribute>("k_max");

    // compute
    if (!smooth_min && !smooth_max)
    {
      hmap::for_each_tile(
          {p_out, p_in},
          [&crange](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_in = p_arrays[1];
            *pa_out = *pa_in;
            hmap::clamp(*pa_out, crange.x, crange.y);
          },
          node.cfg().cm_cpu);
    }
    else
    {
      if (smooth_min)
        hmap::for_each_tile(
            {p_out, p_in},
            [&crange, &k_min](std::vector<hmap::Array *> p_arrays,
                              const hmap::TileRegion &)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_in = p_arrays[1];
              *pa_out = *pa_in;
              hmap::clamp_min_smooth(*pa_out, crange.x, k_min);
            },
            node.cfg().cm_cpu);
      else
        hmap::for_each_tile(
            {p_out, p_in},
            [&crange](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_in = p_arrays[1];
              *pa_out = *pa_in;
              hmap::clamp_min(*pa_out, crange.x);
            },
            node.cfg().cm_cpu);

      if (smooth_max)
        hmap::for_each_tile(
            {p_out, p_in},
            [&crange, &k_max](std::vector<hmap::Array *> p_arrays,
                              const hmap::TileRegion &)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_in = p_arrays[1];
              *pa_out = *pa_in;
              hmap::clamp_max_smooth(*pa_out, crange.y, k_max);
            },
            node.cfg().cm_cpu);
      else
        hmap::for_each_tile(
            {p_out, p_in},
            [&crange](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_in = p_arrays[1];
              *pa_out = *pa_in;
              hmap::clamp_max(*pa_out, crange.y);
            },
            node.cfg().cm_cpu);
    }

    if (node.get_attr<BoolAttribute>("remap"))
      p_out->remap(0.f, 1.f, node.cfg().cm_cpu);

    // post-process
    post_process_heightmap(node, *p_out, p_in);
  }
}

} // namespace hesiod
