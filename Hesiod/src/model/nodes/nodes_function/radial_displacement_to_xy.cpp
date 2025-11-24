/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_radial_displacement_to_xy_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "dr");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "dx", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "dy", CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>("smoothing", "smoothing", 1.f, 0.f, 10.f);
  node.add_attr<Vec2FloatAttribute>("center", "center");

  // attribute(s) order
  node.set_attr_ordered_key({"smoothing", "center"});
}

void compute_radial_displacement_to_xy_node(BaseNode &node)
{
  if (node.compute_started)
    node.compute_finished(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_dr = node.get_value_ref<hmap::Heightmap>("dr");

  if (p_dr)
  {
    hmap::Heightmap *p_dx = node.get_value_ref<hmap::Heightmap>("dx");
    hmap::Heightmap *p_dy = node.get_value_ref<hmap::Heightmap>("dy");

    hmap::transform(
        *p_dr,
        *p_dx,
        *p_dy,
        [&node](hmap::Array &dr, hmap::Array &dx, hmap::Array &dy, hmap::Vec4<float> bbox)
        {
          hmap::radial_displacement_to_xy(dr,
                                          dx,
                                          dy,
                                          node.get_attr<FloatAttribute>("smoothing"),
                                          node.get_attr<Vec2FloatAttribute>("center"),
                                          bbox);
        });
  }

  if (node.compute_finished)
    node.compute_finished(node.get_id());
}

} // namespace hesiod
