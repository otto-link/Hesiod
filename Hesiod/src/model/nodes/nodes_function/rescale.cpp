/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_rescale_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("scaling", 1.f, 0.f, 4.f, "scaling");
  p_node->add_attr<BoolAttribute>("centered", false, "centered");

  // attribute(s) order
  p_node->set_attr_ordered_key({"scaling", "centered"});
}

void compute_rescale_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");

    // copy the input heightmap
    *p_out = *p_in;

    float vref = 0.f;

    if (GET("centered", BoolAttribute))
      vref = p_out->mean();

    hmap::transform(*p_out,
                    [p_node, &vref](hmap::Array &x)
                    { hmap::rescale(x, GET("scaling", FloatAttribute), vref); });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
