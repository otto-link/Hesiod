/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_voronoi_fbm_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<EnumAttribute>("return_type",
                               "return_type",
                               enum_mappings.voronoi_return_type_map,
                               "F1: squared distance to the closest point");
  node.add_attr<WaveNbAttribute>("kw", "kw");

  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<FloatAttribute>("jitter.x", "jitter.x", 1.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("jitter.y", "jitter.y", 1.f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("k_smoothing", "k_smoothing", 0.5f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("exp_sigma", "exp_sigma", 0.1f, 0.f, 0.3f);

  node.add_attr<BoolAttribute>("sqrt_output", "sqrt_output", false);

  node.add_attr<IntAttribute>("octaves", "octaves", 8, 0, 32);

  node.add_attr<FloatAttribute>("weight", "weight", 0.7f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("persistence", "persistence", 0.5f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("lacunarity", "lacunarity", 2.f, 0.01f, 4.f);

  // attribute(s) order
  node.set_attr_ordered_key({"return_type",
                             "_SEPARATOR_",
                             "kw",
                             "seed",
                             "jitter.x",
                             "jitter.y",
                             "k_smoothing",
                             "exp_sigma",
                             "sqrt_output",
                             "octaves",
                             "weight",
                             "persistence",
                             "lacunarity"});

  setup_post_process_heightmap_attributes(node);
}

void compute_voronoi_fbm_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base voronoi function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  hmap::transform(
      {p_out, p_ctrl, p_dx, p_dy},
      [&node](std::vector<hmap::Array *> p_arrays,
              hmap::Vec2<int>            shape,
              hmap::Vec4<float>          bbox)
      {
        hmap::Array *pa_out = p_arrays[0];
        hmap::Array *pa_ctrl = p_arrays[1];
        hmap::Array *pa_dx = p_arrays[2];
        hmap::Array *pa_dy = p_arrays[3];

        hmap::VoronoiReturnType rtype = (hmap::VoronoiReturnType)
                                            node.get_attr<EnumAttribute>("return_type");

        hmap::Vec2<float> jitter(node.get_attr<FloatAttribute>("jitter.x"),
                                 node.get_attr<FloatAttribute>("jitter.y"));

        *pa_out = hmap::gpu::voronoi_fbm(shape,
                                         node.get_attr<WaveNbAttribute>("kw"),
                                         node.get_attr<SeedAttribute>("seed"),
                                         jitter,
                                         node.get_attr<FloatAttribute>("k_smoothing"),
                                         node.get_attr<FloatAttribute>("exp_sigma"),
                                         rtype,
                                         node.get_attr<IntAttribute>("octaves"),
                                         node.get_attr<FloatAttribute>("weight"),
                                         node.get_attr<FloatAttribute>("persistence"),
                                         node.get_attr<FloatAttribute>("lacunarity"),
                                         pa_ctrl,
                                         pa_dx,
                                         pa_dy,
                                         bbox);
      },
      node.get_config_ref()->hmap_transform_mode_gpu);

  // apply square root
  p_out->remap();

  if (node.get_attr<BoolAttribute>("sqrt_output"))
    hmap::transform(
        {p_out},
        [](std::vector<hmap::Array *> p_arrays)
        {
          hmap::Array *pa_out = p_arrays[0];
          *pa_out = hmap::sqrt(*pa_out);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
