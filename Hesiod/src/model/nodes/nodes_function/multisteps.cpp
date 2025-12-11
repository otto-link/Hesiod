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

void setup_multisteps_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "control");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("angle", "Rotation Angle", 0.f, -180.f, 180.f, "{:.1f}°");
  node.add_attr<FloatAttribute>("r", "Step Width Ratio", 1.4f, 0.01f, 3.f);
  node.add_attr<IntAttribute>("nsteps", "Number of Steps", 5, 1, 32);
  node.add_attr<FloatAttribute>("scale", "Overall Scale", 0.5f, 0.01f, 2.f);
  node.add_attr<Vec2FloatAttribute>("center", "Center");
  node.add_attr<FloatAttribute>("outer_slope", "Outer Slope", 0.05f, 0.f, 1.f);

  node.add_attr<FloatAttribute>("elevation_exponent",
                                "Elevation Exponent",
                                0.7f,
                                0.01f,
                                2.f);
  node.add_attr<FloatAttribute>("shape_gain", "Transition Shape Gain", 4.f, 0.01f, 8.f);

  node.add_attr<SeedAttribute>("seed", "Seed");
  node.add_attr<WaveNbAttribute>("kw", "Spatial Frequency");
  node.add_attr<FloatAttribute>("noise_amp", "Amplitude", 0.1f, 0.f, 1.f);
  node.add_attr<FloatAttribute>("noise_rugosity", "Smoothness", 0.f, 0.f, 1.f);
  node.add_attr<BoolAttribute>("noise_inflate", "", "Inflate", "Deflate", true);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Geometry",
                             "angle",
                             "r",
                             "nsteps",
                             "scale",
                             "center",
                             "outer_slope",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Shape Controls",
                             "elevation_exponent",
                             "shape_gain",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Noise Parameters",
                             "seed",
                             "kw",
                             "noise_amp",
                             "noise_rugosity",
                             "noise_inflate",
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node);
}

void compute_multisteps_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_ctrl = node.get_value_ref<hmap::Heightmap>("control");
  hmap::Heightmap *p_env = node.get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");

  if (p_dx || p_dy)
  {
    // use input noise
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

          *pa_out = hmap::multisteps(shape,
                                     node.get_attr<FloatAttribute>("angle"),
                                     node.get_attr<FloatAttribute>("r"),
                                     node.get_attr<IntAttribute>("nsteps"),
                                     node.get_attr<FloatAttribute>("elevation_exponent"),
                                     node.get_attr<FloatAttribute>("shape_gain"),
                                     node.get_attr<FloatAttribute>("scale"),
                                     node.get_attr<FloatAttribute>("outer_slope"),
                                     pa_ctrl,
                                     pa_dx,
                                     pa_dy,
                                     node.get_attr<Vec2FloatAttribute>("center"),
                                     bbox);
        },
        node.get_config_ref()->hmap_transform_mode_cpu);
  }
  else
  {
    // use built-in noise
    hmap::transform(
        {p_out, p_ctrl},
        [&node](std::vector<hmap::Array *> p_arrays,
                hmap::Vec2<int>            shape,
                hmap::Vec4<float>          bbox)
        {
          hmap::Array *pa_out = p_arrays[0];
          hmap::Array *pa_ctrl = p_arrays[1];

          *pa_out = hmap::gpu::multisteps(
              shape,
              node.get_attr<FloatAttribute>("angle"),
              node.get_attr<SeedAttribute>("seed"),
              node.get_attr<WaveNbAttribute>("kw"),
              node.get_attr<FloatAttribute>("noise_amp"),
              node.get_attr<FloatAttribute>("noise_rugosity"),
              node.get_attr<BoolAttribute>("noise_inflate"),
              node.get_attr<FloatAttribute>("r"),
              node.get_attr<IntAttribute>("nsteps"),
              node.get_attr<FloatAttribute>("elevation_exponent"),
              node.get_attr<FloatAttribute>("shape_gain"),
              node.get_attr<FloatAttribute>("scale"),
              node.get_attr<FloatAttribute>("outer_slope"),
              pa_ctrl,
              node.get_attr<Vec2FloatAttribute>("center"),
              bbox);
        },
        node.get_config_ref()->hmap_transform_mode_gpu);
  }

  // post-process
  post_apply_enveloppe(node, *p_out, p_env);
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
