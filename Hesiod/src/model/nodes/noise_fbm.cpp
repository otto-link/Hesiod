/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/primitives.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/primitives.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

NoiseFbm::NoiseFbm(std::shared_ptr<ModelConfig> config) : BaseNode("NoiseFbm", config)
{
  LOG->trace("NoiseFbm::NoiseFbm");

  // input port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "dx");
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "dy");
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "control");
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "envelope");

  // output port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "out",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);

  // attribute(s)
  this->attr["noise_type"] = NEW(MapEnumAttribute, noise_type_map, "Noise type");
  this->attr["kw"] = NEW(WaveNbAttribute);
  this->attr["seed"] = NEW(SeedAttribute);
  this->attr["octaves"] = NEW(IntAttribute, 8, 0, 32, "Octaves");
  this->attr["weight"] = NEW(FloatAttribute, 0.7f, 0.f, 1.f, "Weight");
  this->attr["persistence"] = NEW(FloatAttribute, 0.5f, 0.f, 1.f, "Persistence");
  this->attr["lacunarity"] = NEW(FloatAttribute, 2.f, 0.01f, 4.f, "Lacunarity");
  this->attr["inverse"] = NEW(BoolAttribute, false, "Inverse");
  this->attr["remap"] = NEW(RangeAttribute, "Remap range");

  // attribute(s) order
  this->attr_ordered_key = {"noise_type",
                            "_SEPARATOR_",
                            "kw",
                            "seed",
                            "octaves",
                            "weight",
                            "persistence",
                            "lacunarity",
                            "_SEPARATOR_",
                            "inverse",
                            "remap"};
}

void NoiseFbm::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  LOG->trace("computing node {}", this->get_label());

  // base noise function
  hmap::HeightMap *p_dx = this->get_value_ref<hmap::HeightMap>("dx");
  hmap::HeightMap *p_dy = this->get_value_ref<hmap::HeightMap>("dy");
  hmap::HeightMap *p_ctrl = this->get_value_ref<hmap::HeightMap>("control");
  hmap::HeightMap *p_env = this->get_value_ref<hmap::HeightMap>("envelope");
  hmap::HeightMap *p_out = this->get_value_ref<hmap::HeightMap>("out");

  hmap::fill(*p_out,
             p_dx,
             p_dy,
             p_ctrl,
             [this](hmap::Vec2<int>   shape,
                    hmap::Vec4<float> bbox,
                    hmap::Array      *p_noise_x,
                    hmap::Array      *p_noise_y,
                    hmap::Array      *p_ctrl)
             {
               return hmap::noise_fbm(
                   (hmap::NoiseType)GET("noise_type", MapEnumAttribute),
                   shape,
                   GET("kw", WaveNbAttribute),
                   GET("seed", SeedAttribute),
                   GET("octaves", IntAttribute),
                   GET("weight", FloatAttribute),
                   GET("persistence", FloatAttribute),
                   GET("lacunarity", FloatAttribute),
                   p_ctrl,
                   p_noise_x,
                   p_noise_y,
                   nullptr,
                   bbox);
             });

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

  Q_EMIT this->compute_finished(this->get_id());
}

} // namespace hesiod
