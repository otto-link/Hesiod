/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/heightmap.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_noise_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dx");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "dy");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "envelope");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  p_node->add_attr<MapEnumAttribute>("noise_type", noise_type_map, "Noise type");
  p_node->add_attr<WaveNbAttribute>("kw");
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<BoolAttribute>("inverse", false, "Inverse");
  p_node->add_attr<RangeAttribute>("remap", "Remap range");
  p_node->add_attr<BoolAttribute>("GPU", true, "GPU");

  // attribute(s) order
  p_node->set_attr_ordered_key({"noise_type",
                                "_SEPARATOR_",
                                "kw",
                                "seed",
                                "_SEPARATOR_",
                                "inverse",
                                "remap",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_noise_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  // base noise function
  hmap::Heightmap *p_dx = p_node->get_value_ref<hmap::Heightmap>("dx");
  hmap::Heightmap *p_dy = p_node->get_value_ref<hmap::Heightmap>("dy");
  hmap::Heightmap *p_env = p_node->get_value_ref<hmap::Heightmap>("envelope");
  hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("out");

  if (GET("GPU", BoolAttribute))
  {
    hmap::Array dx_array = p_dx ? p_dx->to_array() : hmap::Array();
    hmap::Array dy_array = p_dy ? p_dy->to_array() : hmap::Array();

    hmap::Array out_array = hmap::gpu::noise(
        (hmap::NoiseType)GET("noise_type", MapEnumAttribute),
        p_out->shape,
        GET("kw", WaveNbAttribute),
        GET("seed", SeedAttribute),
        p_dx ? &dx_array : nullptr,
        p_dy ? &dy_array : nullptr);

    p_out->from_array_interp_nearest(out_array);
  }
  else
  {
    hmap::fill(*p_out,
               p_dx,
               p_dy,
               [p_node](hmap::Vec2<int>   shape,
                        hmap::Vec4<float> bbox,
                        hmap::Array      *p_noise_x,
                        hmap::Array      *p_noise_y)
               {
                 return hmap::noise((hmap::NoiseType)GET("noise_type", MapEnumAttribute),
                                    shape,
                                    GET("kw", WaveNbAttribute),
                                    GET("seed", SeedAttribute),
                                    p_noise_x,
                                    p_noise_y,
                                    nullptr,
                                    bbox);
               });
  }

  // add envelope
  if (p_env)
  {
    float hmin = p_out->min();
    hmap::transform(*p_out,
                    *p_env,
                    [&hmin](hmap::Array &a, hmap::Array &b)
                    {
                      a -= hmin;
                      a *= b;
                    });
  }

  // post-process
  post_process_heightmap(*p_out,
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
