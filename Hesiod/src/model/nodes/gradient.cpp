/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/gradient.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/math.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

Gradient::Gradient(std::shared_ptr<ModelConfig> config) : BaseNode("Gradient", config)
{
  LOG->trace("Gradient::Gradient");

  // input port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");

  // output port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "dx",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "dy",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);

  // attribute(s)
  this->attr["remap"] = NEW(RangeAttribute, "Remap range");

  // attribute(s) order
}

void Gradient::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  LOG->trace("computing node {}", this->get_label());

  hmap::HeightMap *p_in = this->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_dx = this->get_value_ref<hmap::HeightMap>("dx");
    hmap::HeightMap *p_dy = this->get_value_ref<hmap::HeightMap>("dy");

    hmap::transform(*p_dx,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_x(in, out); });

    hmap::transform(*p_dy,
                    *p_in,
                    [](hmap::Array &out, hmap::Array &in) { hmap::gradient_y(in, out); });

    p_dx->smooth_overlap_buffers();
    p_dy->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_dx,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));

    post_process_heightmap(*p_dy,
                           false, // inverse
                           false, // smooth
                           0,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           GET_ATTR("remap", RangeAttribute, is_active),
                           GET("remap", RangeAttribute));
  }

  Q_EMIT this->compute_finished(this->get_id());
}

} // namespace hesiod
