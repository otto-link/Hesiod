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

void setup_wave_dune_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "kw", 2.f, 0.01f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "angle", 0.f, -180.f, 180.f);
  ADD_ATTR(node, FloatAttribute, "xtop", 0.7f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "xbottom", 0.9f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "phase_shift", 0.f, -180.f, 180.f);

  // attribute(s) order
  node.set_attr_ordered_key({"kw", "angle", "xtop", "xbottom", "phase_shift"});

  setup_post_process_heightmap_attributes(node);
}

void compute_wave_dune_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

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

        *pa_out = hmap::wave_dune(shape,
                                  GET(node, "kw", FloatAttribute),
                                  GET(node, "angle", FloatAttribute),
                                  GET(node, "xtop", FloatAttribute),
                                  GET(node, "xbottom", FloatAttribute),
                                  GET(node, "phase_shift", FloatAttribute),
                                  pa_dr,
                                  nullptr,
                                  nullptr,
                                  bbox);
      },
      node.get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
