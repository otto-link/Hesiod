/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_badlands_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  ADD_ATTR(node, FloatAttribute, "elevation", 0.4f, 0.f, 1.f);
  ADD_ATTR(node, WaveNbAttribute, "kw");
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(node, FloatAttribute, "rugosity", 0.2f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "angle", 30.f, -180.f, 180.f);
  ADD_ATTR(node, FloatAttribute, "k_smoothing", 0.1f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "base_noise_amp", 0.2f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"elevation",
                             "kw",
                             "seed",
                             "octaves",
                             "_SEPARATOR_",
                             "rugosity",
                             "angle",
                             "k_smoothing",
                             "base_noise_amp"});

  setup_post_process_heightmap_attributes(node);

  // disable post-processing remap by default
  GET_REF(node, "post_remap", RangeAttribute)->set_is_active(false);
  GET_REF(node, "post_remap", RangeAttribute)->save_initial_state();
}

void compute_badlands_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base badlands function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");

  hmap::transform(
      {p_out, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];

        *pa_out = hmap::gpu::badlands(shape,
                                      GET(node, "kw", WaveNbAttribute),
                                      GET(node, "seed", SeedAttribute),
                                      GET(node, "octaves", IntAttribute),
                                      GET(node, "rugosity", FloatAttribute),
                                      GET(node, "angle", FloatAttribute),
                                      GET(node, "k_smoothing", FloatAttribute),
                                      GET(node, "base_noise_amp", FloatAttribute),
                                      pa_dx,
                                      pa_dy,
                                      bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  p_out->remap(0.f, GET(node, "elevation", FloatAttribute));

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
