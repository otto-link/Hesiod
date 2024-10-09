/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/filters.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

using namespace attr;

namespace hesiod
{

void setup_gamma_correction_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "output", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("gamma", 2.f, 0.01f, 10.f, "Gamma");
}

void compute_gamma_correction_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_mask = p_node->get_value_ref<hmap::HeightMap>("mask");
    hmap::HeightMap *p_out = p_node->get_value_ref<hmap::HeightMap>("output");

    *p_out = *p_in;

    float hmin = p_out->min();
    float hmax = p_out->max();

    p_out->remap(0.f, 1.f, hmin, hmax);

    hmap::transform(*p_out,
                    p_mask,
                    [p_node](hmap::Array &x, hmap::Array *p_mask)
                    { hmap::gamma_correction(x, GET("gamma", FloatAttribute), p_mask); });

    p_out->remap(hmin, hmax, 0.f, 1.f);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
