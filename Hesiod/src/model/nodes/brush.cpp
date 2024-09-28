/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/authoring.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

Brush::Brush(std::shared_ptr<ModelConfig> config) : BaseNode("Brush", config)
{
  LOG->trace("Brush::Brush");

  // input port(s)

  // output port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::OUT,
                                  "out",
                                  config->shape,
                                  config->tiling,
                                  config->overlap);

  // attribute(s)
  this->attr["hmap"] = NEW(ArrayAttribute, "Heightmap", hmap::Vec2<int>(512, 512));
  this->attr["inverse"] = NEW(BoolAttribute, false, "Inverse");
  this->attr["remap"] = NEW(RangeAttribute, "Remap range");

  // attribute(s) order
  this->attr_ordered_key = {"hmap", "inverse", "remap"};
}

void Brush::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  LOG->trace("computing node {}", this->get_label());

  // base noise function
  hmap::HeightMap *p_out = this->get_value_ref<hmap::HeightMap>("out");

  hmap::Array array = GET("hmap", ArrayAttribute);
  p_out->from_array_interp(array);

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
