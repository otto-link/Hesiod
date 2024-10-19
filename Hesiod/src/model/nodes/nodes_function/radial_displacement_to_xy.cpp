/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_radial_displacement_to_xy_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::HeightMap>(gnode::PortType::IN, "dr");
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "dx", CONFIG);
  p_node->add_port<hmap::HeightMap>(gnode::PortType::OUT, "dy", CONFIG);

  // attribute(s)
  p_node->add_attr<FloatAttribute>("smoothing", 1.f, 0.f, 10.f, "smoothing");
  p_node->add_attr<Vec2FloatAttribute>("center", "center");

  // attribute(s) order
  p_node->set_attr_ordered_key({"smoothing", "center"});
}

void compute_radial_displacement_to_xy_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::HeightMap *p_dr = p_node->get_value_ref<hmap::HeightMap>("dr");

  if (p_dr)
  {
    hmap::HeightMap *p_dx = p_node->get_value_ref<hmap::HeightMap>("dx");
    hmap::HeightMap *p_dy = p_node->get_value_ref<hmap::HeightMap>("dy");

    hmap::transform(*p_dr,
                    *p_dx,
                    *p_dy,
                    [p_node](hmap::Array      &dr,
                             hmap::Array      &dx,
                             hmap::Array      &dy,
                             hmap::Vec4<float> bbox)
                    {
                      hmap::radial_displacement_to_xy(dr,
                                                      dx,
                                                      dy,
                                                      GET("smoothing", FloatAttribute),
                                                      GET("center", Vec2FloatAttribute),
                                                      bbox);
                    });
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
