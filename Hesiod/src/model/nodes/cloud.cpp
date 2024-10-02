/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/cloud.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/geometry.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

Cloud::Cloud(std::shared_ptr<ModelConfig> config) : BaseNode("Cloud", config)
{
  LOG->trace("Cloud::Cloud");

  // input port(s)

  // output port(s)
  this->add_port<hmap::Cloud>(gnode::PortType::OUT, "cloud");

  // attribute(s)
  this->attr["cloud"] = NEW(CloudAttribute, "Cloud");

  // attribute(s) order
}

void Cloud::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  LOG->trace("computing node {}", this->get_label());

  hmap::Cloud *p_out = this->get_value_ref<hmap::Cloud>("cloud");
  *p_out = GET("cloud", CloudAttribute);

  Q_EMIT this->compute_finished(this->get_id());
}

} // namespace hesiod
