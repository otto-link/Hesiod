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

void setup_hemisphere_field_fbm_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "density");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "size");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  ADD_ATTR(WaveNbAttribute, "kw");
  ADD_ATTR(SeedAttribute, "seed");
  ADD_ATTR(FloatAttribute, "rmin", 0.05f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "rmax", 0.8f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "amplitude_random_ratio", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "shift", 0.1f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "density", 0.1f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "jitter.x", 1.f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "jitter.y", 1.f, 0.f, 1.f);
  ADD_ATTR(IntAttribute, "octaves", 8, 0, 32);
  ADD_ATTR(FloatAttribute, "persistence", 0.5f, 0.f, 1.f);
  ADD_ATTR(FloatAttribute, "lacunarity", 2.f, 0.01f, 4.f);

  // attribute(s) order
  p_node->set_attr_ordered_key({"kw",
                                "seed",
                                "rmin",
                                "rmax",
                                "amplitude_random_ratio",
                                "shift",
                                "density",
                                "jitter.x",
                                "jitter.y",
                                "_SEPARATOR_",
                                "octaves",
                                "persistence",
                                "lacunarity"});

  setup_post_process_heightmap_attributes(p_node);
}

void compute_hemisphere_field_fbm_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  // AppContext &ctx = HSD_CTX;

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_dr = p_node->get_value_ref<hmap::Heightmap>("dr");
  hmap::Heightmap *p_density = p_node->get_value_ref<hmap::Heightmap>("density");
  hmap::Heightmap *p_size = p_node->get_value_ref<hmap::Heightmap>("size");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_dx, p_dy, p_dr, p_density, p_size},
      [p_node](std::vector<hmap::Array *> p_arrays,
               hmap::Vec2<int>            shape,
               hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_dx = p_arrays[1];
        hmap::Array *pa_dy = p_arrays[2];
        hmap::Array *pa_dr = p_arrays[3];
        hmap::Array *pa_density = p_arrays[4];
        hmap::Array *pa_size = p_arrays[5];

        hmap::Vec2<float> jitter(GET("jitter.x", FloatAttribute),
                                 GET("jitter.y", FloatAttribute));

        *pa_out = hmap::gpu::hemisphere_field_fbm(
            shape,
            GET("kw", WaveNbAttribute),
            GET("seed", SeedAttribute),
            GET("rmin", FloatAttribute),
            GET("rmax", FloatAttribute),
            GET("amplitude_random_ratio", FloatAttribute),
            GET("density", FloatAttribute),
            jitter,
            GET("shift", FloatAttribute),
            GET("octaves", IntAttribute),
            GET("persistence", FloatAttribute),
            GET("lacunarity", FloatAttribute),
            pa_dx,
            pa_dy,
            pa_dr,
            pa_density,
            pa_size,
            bbox);
      },
      HSD_GPU_MODE);

  // post-process
  post_apply_enveloppe(p_node, *p_out, p_env);
  post_process_heightmap(p_node, *p_out);

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
