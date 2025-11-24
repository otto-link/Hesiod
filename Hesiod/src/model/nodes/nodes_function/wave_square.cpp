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

void setup_wave_square_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("kw", "kw", 2.f, 0.01f, FLT_MAX);
  node.add_attr<FloatAttribute>("angle", "angle", 0.f, 0.f, 180.f);
  node.add_attr<FloatAttribute>("phase_shift", "phase_shift", 0.f, -1.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"kw", "angle", "phase_shift"});

  setup_post_process_heightmap_attributes(node);
}

void compute_wave_square_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dr},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dr = p_arrays[1];

        *pa_out = hmap::wave_square(shape,
                                    node.get_attr<FloatAttribute>("kw"),
                                    node.get_attr<FloatAttribute>("angle"),
                                    node.get_attr<FloatAttribute>("phase_shift"),
                                    pa_dr,
                                    nullptr,
                                    nullptr,
                                    bbox);
      },
      node.get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
