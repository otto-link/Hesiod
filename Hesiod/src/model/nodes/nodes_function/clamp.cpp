/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_clamp_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<RangeAttribute>("clamp", "clamp");
  p_node->add_attr<BoolAttribute>("smooth_min", false, "smooth_min");
  p_node->add_attr<FloatAttribute>("k_min", 0.05f, 0.01f, 1.f, "k_min");
  p_node->add_attr<BoolAttribute>("smooth_max", false, "smooth_max");
  p_node->add_attr<FloatAttribute>("k_max", 0.05f, 0.01f, 1.f, "k_max");
  p_node->add_attr<BoolAttribute>("remap", false, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key(
      {"clamp", "smooth_min", "k_min", "smooth_max", "k_max", "remap"});
}

void compute_clamp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    // retrieve parameters
    hmap::Vec2<float> crange = GET("clamp", RangeAttribute);
    bool              smooth_min = GET("smooth_min", BoolAttribute);
    bool              smooth_max = GET("smooth_max", BoolAttribute);
    float             k_min = GET("k_min", FloatAttribute);
    float             k_max = GET("k_max", FloatAttribute);

    // compute
    if (!smooth_min && !smooth_max)
    {
      hmap::transform(
          {p_out},
          [&crange](std::vector<hmap::Array *> p_arrays,
                    hmap::Vec2<int>,
                    hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::clamp(*pa_out, crange.x, crange.y);
          },
          p_node->get_config_ref()->hmap_transform_mode_cpu);
    }
    else
    {
      if (smooth_min)
        hmap::transform(
            {p_out},
            [&crange, &k_min](std::vector<hmap::Array *> p_arrays,
                              hmap::Vec2<int>,
                              hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_min_smooth(*pa_out, crange.x, k_min);
            },
            p_node->get_config_ref()->hmap_transform_mode_cpu);
      else
        hmap::transform(
            {p_out},
            [&crange](std::vector<hmap::Array *> p_arrays,
                      hmap::Vec2<int>,
                      hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_min(*pa_out, crange.x);
            },
            p_node->get_config_ref()->hmap_transform_mode_cpu);

      if (smooth_max)
        hmap::transform(
            {p_out},
            [&crange, &k_max](std::vector<hmap::Array *> p_arrays,
                              hmap::Vec2<int>,
                              hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_max_smooth(*pa_out, crange.y, k_max);
            },
            p_node->get_config_ref()->hmap_transform_mode_cpu);
      else
        hmap::transform(
            {p_out},
            [&crange](std::vector<hmap::Array *> p_arrays,
                      hmap::Vec2<int>,
                      hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::clamp_max(*pa_out, crange.y);
            },
            p_node->get_config_ref()->hmap_transform_mode_cpu);
    }

    if (GET("remap", BoolAttribute))
      p_out->remap();
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
