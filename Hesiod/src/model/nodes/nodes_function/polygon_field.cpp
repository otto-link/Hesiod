/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_polygon_field_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "size");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(node, WaveNbAttribute, "kw");
  ADD_ATTR(node, SeedAttribute, "seed");
  ADD_ATTR(node, FloatAttribute, "rmin", 0.05f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "rmax", 0.8f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "clamping_dist", 0.1f, 0.f, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "clamping_k", 0.01f, 0.f, 0.2f);
  ADD_ATTR(node, FloatAttribute, "shift", 0.1f, 0.f, 1.f);
  ADD_ATTR(node, IntAttribute, "n_vertices_min", 3, 3, 64);
  ADD_ATTR(node, IntAttribute, "n_vertices_max", 8, 3, 64);
  ADD_ATTR(node, FloatAttribute, "density", 0.1f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "jitter.x", 1.f, 0.f, 1.f);
  ADD_ATTR(node, FloatAttribute, "jitter.y", 1.f, 0.f, 1.f);

  // attribute(s) order
  node.set_attr_ordered_key({"kw",
                             "seed",
                             "rmin",
                             "rmax",
                             "clamping_dist",
                             "clamping_k",
                             "shift",
                             "n_vertices_min",
                             "n_vertices_max",
                             "density",
                             "jitter.x",
                             "jitter.y"});

  setup_post_process_heightmap_attributes(node);
}

void compute_polygon_field_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_density = node.get_value_ref<hmap::Heightmap>("density");
  hmap::Heightmap *p_size = node.get_value_ref<hmap::Heightmap>("size");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dx, p_dy, p_dr, p_density, p_size},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];
        hmap::Array *pa_dr = p_arrays[3];
        hmap::Array *pa_density = p_arrays[4];
        hmap::Array *pa_size = p_arrays[5];

        hmap::Vec2<float> jitter(GET(node, "jitter.x", FloatAttribute),
                                 GET(node, "jitter.y", FloatAttribute));

        *pa_out = hmap::gpu::polygon_field(shape,
                                           GET(node, "kw", WaveNbAttribute),
                                           GET(node, "seed", SeedAttribute),
                                           GET(node, "rmin", FloatAttribute),
                                           GET(node, "rmax", FloatAttribute),
                                           GET(node, "clamping_dist", FloatAttribute),
                                           GET(node, "clamping_k", FloatAttribute),
                                           GET(node, "n_vertices_min", IntAttribute),
                                           GET(node, "n_vertices_max", IntAttribute),
                                           GET(node, "density", FloatAttribute),
                                           jitter,
                                           GET(node, "shift", FloatAttribute),
                                           pa_dx,
                                           pa_dy,
                                           pa_dr,
                                           pa_density,
                                           pa_size,
                                           bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
