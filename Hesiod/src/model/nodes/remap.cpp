/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/range.hpp"

namespace hesiod
{

Remap::Remap(std::shared_ptr<ModelConfig> config) : BaseNode("Remap", config)
{
  HLOG->trace("Remap::Remap");

  // categories
  this->category = "Filter/Range";

  // input port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");

  // output port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "output",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);

  // attribute(s)
  this->attr["remap"] = create_attr<RangeAttribute>();
}

void Remap::compute()
{
  HLOG->trace("computing node {}", this->get_label());

  // base noise function
  hmap::HeightMap *p_in = this->get_value_ref<hmap::HeightMap>("input");
  hmap::HeightMap *p_out = this->get_value_ref<hmap::HeightMap>("output");

  if (p_in)
  {
    *p_out = *p_in;
    p_out->remap(this->get_attr<RangeAttribute>("remap").x,
                 this->get_attr<RangeAttribute>("remap").y);
  }
}

} // namespace hesiod
