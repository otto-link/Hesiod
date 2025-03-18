/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_white_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(BoolAttribute, "inverse", false);
  ADD_ATTR(RangeAttribute, "remap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed", "_SEPARATOR_", "inverse", "remap"});
}

void compute_white_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  int seed = (int)GET("seed", SeedAttribute);

  hmap::transform(
      {p_out},
      [&seed](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];

        *pa_out = hmap::white(shape, 0.f, 1.f, (uint)seed++);
      },
      p_node->get_config_ref()->hmap_transform_mode_cpu);

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(
        {p_out, p_env},
        [&hmin](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_a = p_arrays[0];
          hmap::Array *pa_b = p_arrays[1];

          *pa_a -= hmin;
          *pa_a *= *pa_b;
        },
        p_node->get_config_ref()->hmap_transform_mode_cpu);
  }

  // post-process
  post_process_heightmap(p_node,
                         *p_out,
                         GET("inverse", BoolAttribute),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR("remap", RangeAttribute, is_active),
                         GET("remap", RangeAttribute));

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
