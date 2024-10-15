/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_lerp_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "a");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "b");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "t");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("t", 0.5f, 0.f, 1.f, "t");
}

void compute_lerp_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_a = p_node->get_value_ref<hmap::HeightMap>("a");
  hmap::HeightMap *p_b = p_node->get_value_ref<hmap::HeightMap>("b");

  if (p_a && p_b)
  {
    hmap::HeightMap *p_t = p_node->get_value_ref<hmap::HeightMap>("t");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    if (p_t)
      hmap::transform(*p_out,
                      *p_a,
                      *p_b,
                      *p_t,
                      [](hmap::Array &out, hmap::Array &a, hmap::Array &b, hmap::Array &t)
                      { out = hmap::lerp(a, b, t); });
    else
      hmap::transform(*p_out,
                      *p_a,
                      *p_b,
                      [p_node](hmap::Array &out, hmap::Array &a, hmap::Array &b)
                      { out = hmap::lerp(a, b, GET("t", FloatAttribute)); });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
