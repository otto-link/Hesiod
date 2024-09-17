/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/filters.hpp"

namespace hesiod
{

GammaCorrection::GammaCorrection(std::shared_ptr<ModelConfig> config)
    : BaseNode("GammaCorrection", config)
{
  HLOG->trace("GammaCorrection::GammaCorrection");

  // input port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");

  // output port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "output",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);

  // attribute(s)
  this->attr["gamma"] = create_attr<FloatAttribute>(2.f, 0.01f, 10.f);
}

void GammaCorrection::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  HLOG->trace("computing node {}", this->get_label());

  // base noise function
  hmap::HeightMap *p_in = this->get_value_ref<hmap::HeightMap>("input");
  hmap::HeightMap *p_mask = this->get_value_ref<hmap::HeightMap>("mask");
  hmap::HeightMap *p_out = this->get_value_ref<hmap::HeightMap>("output");

  if (p_in)
  {
    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    p_out->remap(0.f, 1.f, hmin, hmax);

    hmap::transform(
        *p_out,
        p_mask,
        [this](hmap::Array &x, hmap::Array *p_mask)
        { hmap::gamma_correction(x, this->get_attr<FloatAttribute>("gamma"), p_mask); });

    p_out->remap(hmin, hmax, 0.f, 1.f);
  }

  Q_EMIT this->compute_finished(this->get_id());
}

} // namespace hesiod
