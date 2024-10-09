/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/heightmap.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_remap_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<RangeAttribute>("remap", "Remap range");
}

void compute_remap_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    *p_out = *p_in;
    p_out->remap(GET("remap", RangeAttribute)[0], GET("remap", RangeAttribute)[1]);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
