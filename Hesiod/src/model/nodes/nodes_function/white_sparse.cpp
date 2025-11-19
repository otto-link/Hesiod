/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_white_sparse_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<FloatAttribute>("density", "density", 0.1f, 0.f, 1.f);

  node.add_attr<BoolAttribute>("inverse", "inverse", false);

  node.add_attr<RangeAttribute>("remap", "remap");

  // attribute(s) order
  node.set_attr_ordered_key({"seed", "density", "_SEPARATOR_", "inverse", "remap"});
}

void compute_white_sparse_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  int   seed = node.get_attr<SeedAttribute>("seed");
  float density = node.get_attr<FloatAttribute>("density");
  float density_per_tile = density / (float)p_out->get_ntiles();

  hmap::transform(
      {p_out},
      [&density_per_tile, &seed](std::vector<hmap::Array *> p_arrays,
                                 hmap::Vec2<int>            shape,
                                 hmap::Vec4<float>)
      {
        hmap::Array *pa_out = p_arrays[0];

        *pa_out = hmap::white_sparse(shape, 0.f, 1.f, density_per_tile, (uint)seed++);
      },
      node.get_config_ref()->hmap_transform_mode_cpu);

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
        node.get_config_ref()->hmap_transform_mode_cpu);
  }

  // post-process
  post_process_heightmap(node,
                         *p_out,

                         node.get_attr<BoolAttribute>("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         node.get_attr_ref<RangeAttribute>("remap")->get_is_active(),
                         node.get_attr<RangeAttribute>("remap"));

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
