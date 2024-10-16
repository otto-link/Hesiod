/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_select_multiband3_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "low", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "mid", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "high", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("ratio1", 0.2f, 0.f, 1.f, "ratio1");
  p_node->add_attr<FloatAttribute>("ratio2", 0.5f, 0.f, 1.f, "ratio2");
  p_node->add_attr<FloatAttribute>("overlap", 0.5f, 0.f, 1.f, "overlap");

  // attribute(s) order
  p_node->set_attr_ordered_key({"ratio1", "ratio2", "overlap"});
}

void compute_select_multiband3_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_in = p_node->get_value_ref<hmap::HeightMap>("input");

  if (p_in)
  {
    hmap::HeightMap *p_low = p_node->get_value_ref<hmap::HeightMap>("low");
    hmap::HeightMap *p_mid = p_node->get_value_ref<hmap::HeightMap>("mid");
    hmap::HeightMap *p_high = p_node->get_value_ref<hmap::HeightMap>("high");

    float vmin = p_in->min();
    float vmax = p_in->max();

    hmap::transform(*p_in,
                    *p_low,
                    *p_mid,
                    *p_high,
                    [p_node, vmin, vmax](hmap::Array &in,
                                         hmap::Array &low,
                                         hmap::Array &mid,
                                         hmap::Array &high)
                    {
                      hmap::select_multiband3(in,
                                              low,
                                              mid,
                                              high,
                                              GET("ratio1", FloatAttribute),
                                              GET("ratio2", FloatAttribute),
                                              GET("overlap", FloatAttribute),
                                              vmin,
                                              vmax);
                    });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
